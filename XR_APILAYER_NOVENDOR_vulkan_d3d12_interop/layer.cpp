// MIT License
//
// Copyright(c) 2022 Matthieu Bucchianeri
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "pch.h"

#include "layer.h"
#include "log.h"
#include "util.h"

namespace xr {

    static inline std::string ToString(XrVersion version) {
        return fmt::format("{}.{}.{}", XR_VERSION_MAJOR(version), XR_VERSION_MINOR(version), XR_VERSION_PATCH(version));
    }

} // namespace xr

namespace {

    using namespace vulkan_d3d12_interop;
    using namespace vulkan_d3d12_interop::log;

    using namespace xr::math;

    std::vector<const char*> ParseExtensionString(char* names) {
        std::vector<const char*> list;
        while (*names != 0) {
            list.push_back(names);
            while (*(++names) != 0) {
                if (*names == ' ') {
                    *names++ = '\0';
                    break;
                }
            }
        }
        return list;
    }

    class OpenXrLayer : public vulkan_d3d12_interop::OpenXrApi {
      private:
        enum GfxApi { Vulkan, OpenGL };

        // State associated with an OpenXR session.
        struct Session {
            XrSession xrSession{XR_NULL_HANDLE};
            XrInstance xrInstance{XR_NULL_HANDLE};

            bool isFovMutable{false};

            GfxApi api;

            // We create a D3D12 device that the runtime will be using.
            ComPtr<ID3D12Device> d3dDevice;
            ComPtr<ID3D12CommandQueue> d3dQueue;

            // We store information about the Vulkan/OpenGL device that the app is using.
            VkInstance vkInstance{VK_NULL_HANDLE};
            VkDevice vkDevice{VK_NULL_HANDLE};
            VkPhysicalDevice vkPhysicalDevice{VK_NULL_HANDLE};
            VkPhysicalDeviceMemoryProperties memoryProperties;
            VkQueue vkQueue{VK_NULL_HANDLE};
            HDC glDC{0};
            HGLRC glRC{0};

            // For synchronization between the app and the runtime, we use a fence (which corresponds to a timeline
            // semaphore in Vulkan or just semaphore in OpenGL).
            ComPtr<ID3D12Fence> d3dFence;
            VkSemaphore vkTimelineSemaphore{VK_NULL_HANDLE};
            GLuint glSemaphore{0};
            UINT64 fenceValue{0};

            // Workaround: the AMD driver does not seem to like closing the handle for the shared fence when using
            // OpenGL. We keep it alive for the whole session.
            wil::shared_handle fenceHandleForAMDWorkaround;

            // Vulkan: for layout transitions.
            VkCommandPool vkCmdPool{VK_NULL_HANDLE};
            VkCommandBuffer vkCmdBuffer{VK_NULL_HANDLE};
        };

        struct Swapchain {
            XrSwapchain xrSwapchain{XR_NULL_HANDLE};
            XrSwapchainCreateInfo createInfo;

            // The parent session.
            XrSession xrSession{XR_NULL_HANDLE};

            // We import the memory corresponding to the D3D12 textures that the runtime exposes.
            std::vector<VkDeviceMemory> vkDeviceMemory;
            std::vector<VkImage> vkImage;
            std::vector<GLuint> glMemory;
            std::vector<GLuint> glImage;
        };

        class GlContextSwitch {
          public:
            GlContextSwitch(const Session& sessionState) : m_valid(sessionState.glDC != 0) {
                if (m_valid) {
                    m_glDC = wglGetCurrentDC();
                    m_glRC = wglGetCurrentContext();

                    wglMakeCurrent(sessionState.glDC, sessionState.glRC);

                    // Reset error codes.
                    (void)glGetError();
                }
            }

            ~GlContextSwitch() noexcept(false) {
                if (m_valid) {
                    const auto lastError = glGetError();

                    wglMakeCurrent(m_glDC, m_glRC);

                    CHECK_MSG(lastError == GL_NO_ERROR, fmt::format("OpenGL error: 0x{:x}", lastError));
                }
            }

          private:
            const bool m_valid;
            HDC m_glDC;
            HGLRC m_glRC;
        };

      public:
        OpenXrLayer() = default;

        ~OpenXrLayer() override {
            while (m_sessions.size()) {
                cleanupSession(m_sessions.begin()->second);
                m_sessions.erase(m_sessions.begin());
            }
        }

        XrResult xrCreateInstance(const XrInstanceCreateInfo* createInfo) override {
            if (createInfo->type != XR_TYPE_INSTANCE_CREATE_INFO) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrCreateInstance",
                              TLArg(xr::ToString(createInfo->applicationInfo.apiVersion).c_str(), "ApiVersion"),
                              TLArg(createInfo->applicationInfo.applicationName, "ApplicationName"),
                              TLArg(createInfo->applicationInfo.applicationVersion, "ApplicationVersion"),
                              TLArg(createInfo->applicationInfo.engineName, "EngineName"),
                              TLArg(createInfo->applicationInfo.engineVersion, "EngineVersion"),
                              TLArg(createInfo->createFlags, "CreateFlags"));

            for (uint32_t i = 0; i < createInfo->enabledApiLayerCount; i++) {
                TraceLoggingWrite(
                    g_traceProvider, "xrCreateInstance", TLArg(createInfo->enabledApiLayerNames[i], "ApiLayerName"));
            }
            for (uint32_t i = 0; i < createInfo->enabledExtensionCount; i++) {
                TraceLoggingWrite(
                    g_traceProvider, "xrCreateInstance", TLArg(createInfo->enabledExtensionNames[i], "ExtensionName"));

                const std::string_view ext(createInfo->enabledExtensionNames[i]);
                if (ext == XR_KHR_VULKAN_ENABLE_EXTENSION_NAME || ext == XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME) {
                    m_isVulkanEnabled = true;
                } else if (ext == XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) {
                    m_isOpenGLEnabled = true;
                }
            }

            // Needed to resolve the requested function pointers.
            OpenXrApi::xrCreateInstance(createInfo);

            // Dump the application name and OpenXR runtime information to help debugging customer issues.
            XrInstanceProperties instanceProperties = {XR_TYPE_INSTANCE_PROPERTIES};
            CHECK_XRCMD(xrGetInstanceProperties(GetXrInstance(), &instanceProperties));
            const auto runtimeName = fmt::format("{} {}.{}.{}",
                                                 instanceProperties.runtimeName,
                                                 XR_VERSION_MAJOR(instanceProperties.runtimeVersion),
                                                 XR_VERSION_MINOR(instanceProperties.runtimeVersion),
                                                 XR_VERSION_PATCH(instanceProperties.runtimeVersion));
            TraceLoggingWrite(g_traceProvider, "xrCreateInstance", TLArg(runtimeName.c_str(), "RuntimeName"));
            Log("Application: %s\n", GetApplicationName().c_str());
            Log("Using OpenXR runtime: %s\n", runtimeName.c_str());

            return XR_SUCCESS;
        }

        XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId) override {
            if (getInfo->type != XR_TYPE_SYSTEM_GET_INFO) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrGetSystem",
                              TLPArg(instance, "Instance"),
                              TLArg(xr::ToCString(getInfo->formFactor), "FormFactor"));

            const XrResult result = OpenXrApi::xrGetSystem(instance, getInfo, systemId);
            if (XR_SUCCEEDED(result) && (m_isVulkanEnabled || m_isOpenGLEnabled) &&
                getInfo->formFactor == XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY) {
                if (!isSystemHandled(*systemId)) {
                    // Get the graphics device requirement for this system (if D3D12 is enabled).
                    PFN_xrGetD3D12GraphicsRequirementsKHR xrGetD3D12GraphicsRequirementsKHR = nullptr;
                    if (SUCCEEDED(xrGetInstanceProcAddr(
                            GetXrInstance(),
                            "xrGetD3D12GraphicsRequirementsKHR",
                            reinterpret_cast<PFN_xrVoidFunction*>(&xrGetD3D12GraphicsRequirementsKHR)))) {
                        m_d3d12Requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR;
                        m_d3d12Requirements.next = nullptr;
                        CHECK_XRCMD(
                            xrGetD3D12GraphicsRequirementsKHR(GetXrInstance(), *systemId, &m_d3d12Requirements));
                        TraceLoggingWrite(g_traceProvider,
                                          "xrGetSystem",
                                          TraceLoggingCharArray((char*)&m_d3d12Requirements.adapterLuid,
                                                                sizeof(LUID),
                                                                "D3D12_AdapterLuid"),
                                          TLArg((int)m_d3d12Requirements.minFeatureLevel, "D3D12_MinFeatureLevel"));
                    }

                    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
                    CHECK_XRCMD(OpenXrApi::xrGetSystemProperties(instance, *systemId, &systemProperties));
                    TraceLoggingWrite(g_traceProvider, "xrGetSystem", TLArg(systemProperties.systemName, "SystemName"));
                    Log("Using OpenXR system: %s\n", systemProperties.systemName);
                }

                // Remember the XrSystemId to use.
                m_systemId = *systemId;
            }

            TraceLoggingWrite(g_traceProvider, "xrGetSystem", TLArg((int)*systemId, "SystemId"));

            return result;
        }

        // XR_KHR_vulkan_enable
        XrResult xrGetVulkanInstanceExtensionsKHR(XrInstance instance,
                                                  XrSystemId systemId,
                                                  uint32_t bufferCapacityInput,
                                                  uint32_t* bufferCountOutput,
                                                  char* buffer) override {
            static const std::string_view instanceExtensions =
                "VK_KHR_external_memory_capabilities VK_KHR_external_semaphore_capabilities "
                "VK_KHR_external_fence_capabilities "
                "VK_KHR_get_physical_device_properties2";

            TraceLoggingWrite(g_traceProvider,
                              "xrGetVulkanInstanceExtensionsKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)systemId, "SystemId"),
                              TLArg(bufferCapacityInput, "BufferCapacityInput"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            if (bufferCapacityInput && bufferCapacityInput < instanceExtensions.size()) {
                return XR_ERROR_SIZE_INSUFFICIENT;
            }

            *bufferCountOutput = (uint32_t)instanceExtensions.size() + 1;
            TraceLoggingWrite(
                g_traceProvider, "xrGetVulkanInstanceExtensionsKHR", TLArg(*bufferCountOutput, "BufferCountOutput"));

            if (bufferCapacityInput && buffer) {
                sprintf_s(buffer, bufferCapacityInput, "%s", instanceExtensions.data());
                TraceLoggingWrite(g_traceProvider, "xrGetVulkanInstanceExtensionsKHR", TLArg(buffer, "Extension"));
            }

            return XR_SUCCESS;
        }

        // XR_KHR_vulkan_enable
        XrResult xrGetVulkanDeviceExtensionsKHR(XrInstance instance,
                                                XrSystemId systemId,
                                                uint32_t bufferCapacityInput,
                                                uint32_t* bufferCountOutput,
                                                char* buffer) override {
            static const std::string_view deviceExtensions =
                "VK_KHR_dedicated_allocation VK_KHR_get_memory_requirements2 VK_KHR_bind_memory2 "
                "VK_KHR_external_memory "
                "VK_KHR_external_memory_win32 VK_KHR_timeline_semaphore "
                "VK_KHR_external_semaphore VK_KHR_external_semaphore_win32";

            TraceLoggingWrite(g_traceProvider,
                              "xrGetVulkanDeviceExtensionsKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)systemId, "SystemId"),
                              TLArg(bufferCapacityInput, "BufferCapacityInput"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            if (bufferCapacityInput && bufferCapacityInput < deviceExtensions.size()) {
                return XR_ERROR_SIZE_INSUFFICIENT;
            }

            *bufferCountOutput = (uint32_t)deviceExtensions.size() + 1;
            TraceLoggingWrite(
                g_traceProvider, "xrGetVulkanDeviceExtensionsKHR", TLArg(*bufferCountOutput, "BufferCountOutput"));

            if (bufferCapacityInput && buffer) {
                sprintf_s(buffer, bufferCapacityInput, "%s", deviceExtensions.data());
                TraceLoggingWrite(g_traceProvider, "xrGetVulkanDeviceExtensionsKHR", TLArg(buffer, "Extension"));
            }

            return XR_SUCCESS;
        }

        // XR_KHR_vulkan_enable
        XrResult xrGetVulkanGraphicsDeviceKHR(XrInstance instance,
                                              XrSystemId systemId,
                                              VkInstance vkInstance,
                                              VkPhysicalDevice* vkPhysicalDevice) override {
            TraceLoggingWrite(g_traceProvider,
                              "xrGetVulkanGraphicsDeviceKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)systemId, "SystemId"),
                              TLPArg(vkInstance, "VkInstance"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            uint32_t deviceCount = 0;
            CHECK_VKCMD(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr));
            std::vector<VkPhysicalDevice> devices(deviceCount);
            CHECK_VKCMD(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data()));

            // Match the Vulkan physical device to the adapter LUID returned by the runtime.
            bool found = false;
            for (const VkPhysicalDevice& device : devices) {
                VkPhysicalDeviceIDProperties deviceId{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
                VkPhysicalDeviceProperties2 properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &deviceId};
                vkGetPhysicalDeviceProperties2(device, &properties);

                if (!deviceId.deviceLUIDValid) {
                    continue;
                }

                if (!memcmp(&m_d3d12Requirements.adapterLuid, deviceId.deviceLUID, sizeof(LUID))) {
                    TraceLoggingWrite(g_traceProvider,
                                      "xrGetVulkanDeviceExtensionsKHR",
                                      TLArg(properties.properties.deviceName, "DeviceName"),
                                      TLArg((int)properties.properties.deviceType, "DeviceType"),
                                      TLArg(properties.properties.vendorID, "VendorId"));
                    Log("Using Vulkan on adapter: %s\n", properties.properties.deviceName);

                    TraceLoggingWrite(
                        g_traceProvider, "xrGetVulkanDeviceExtensionsKHR", TLPArg(device, "VkPhysicalDevice"));
                    *vkPhysicalDevice = device;
                    found = true;
                    break;
                }
            }

            return found ? XR_SUCCESS : XR_ERROR_RUNTIME_FAILURE;
        }

        // XR_KHR_vulkan_enable2
        // This wrapper is adapted from Khronos SDK's Vulkan plugin.
        XrResult xrCreateVulkanInstanceKHR(XrInstance instance,
                                           const XrVulkanInstanceCreateInfoKHR* createInfo,
                                           VkInstance* vulkanInstance,
                                           VkResult* vulkanResult) override {
            if (createInfo->type != XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrCreateVulkanInstanceKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)createInfo->systemId, "SystemId"),
                              TLArg((int)createInfo->createFlags, "CreateFlags"),
                              TLPArg(createInfo->pfnGetInstanceProcAddr, "GetInstanceProcAddr"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(createInfo->systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            uint32_t extensionNamesSize = 0;
            CHECK_XRCMD(
                xrGetVulkanInstanceExtensionsKHR(instance, createInfo->systemId, 0, &extensionNamesSize, nullptr));
            std::vector<char> extensionNames(extensionNamesSize);
            CHECK_XRCMD(xrGetVulkanInstanceExtensionsKHR(
                instance, createInfo->systemId, extensionNamesSize, &extensionNamesSize, &extensionNames[0]));

            // Note: This cannot outlive the extensionNames above, since it's just a collection of views into that
            // string!
            std::vector<const char*> extensions = ParseExtensionString(&extensionNames[0]);

            // Merge the runtime's request with the applications requests
            for (uint32_t i = 0; i < createInfo->vulkanCreateInfo->enabledExtensionCount; ++i) {
                extensions.push_back(createInfo->vulkanCreateInfo->ppEnabledExtensionNames[i]);
            }

            for (uint32_t i = 0; i < extensions.size(); i++) {
                TraceLoggingWrite(g_traceProvider, "xrCreateVulkanInstanceKHR", TLArg(extensions[i], "Extension"));
            }

            VkInstanceCreateInfo instInfo = *createInfo->vulkanCreateInfo;
            instInfo.enabledExtensionCount = (uint32_t)extensions.size();
            instInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

            auto pfnCreateInstance =
                (PFN_vkCreateInstance)createInfo->pfnGetInstanceProcAddr(nullptr, "vkCreateInstance");
            *vulkanResult = pfnCreateInstance(&instInfo, createInfo->vulkanAllocator, vulkanInstance);

            TraceLoggingWrite(g_traceProvider,
                              "xrCreateVulkanInstanceKHR",
                              TLPArg(*vulkanInstance, "VkInstance"),
                              TLArg((int)*vulkanResult, "VkResult"));

            m_vkBootstrapInstance = *vulkanInstance;

            return XR_SUCCESS;
        }

        // XR_KHR_vulkan_enable2
        // This wrapper is adapted from Khronos SDK's Vulkan plugin.
        XrResult xrCreateVulkanDeviceKHR(XrInstance instance,
                                         const XrVulkanDeviceCreateInfoKHR* createInfo,
                                         VkDevice* vulkanDevice,
                                         VkResult* vulkanResult) override {
            if (createInfo->type != XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "XrVulkanDeviceCreateInfoKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)createInfo->systemId, "SystemId"),
                              TLArg((int)createInfo->createFlags, "CreateFlags"),
                              TLPArg(createInfo->pfnGetInstanceProcAddr, "GetInstanceProcAddr"),
                              TLPArg(createInfo->vulkanPhysicalDevice, "VkPhysicalDevice"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(createInfo->systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            uint32_t deviceExtensionNamesSize = 0;
            CHECK_XRCMD(
                xrGetVulkanDeviceExtensionsKHR(instance, createInfo->systemId, 0, &deviceExtensionNamesSize, nullptr));
            std::vector<char> deviceExtensionNames(deviceExtensionNamesSize);
            CHECK_XRCMD(xrGetVulkanDeviceExtensionsKHR(instance,
                                                       createInfo->systemId,
                                                       deviceExtensionNamesSize,
                                                       &deviceExtensionNamesSize,
                                                       &deviceExtensionNames[0]));
            // Note: This cannot outlive the extensionNames above, since it's just a collection of views into that
            // string!
            std::vector<const char*> extensions = ParseExtensionString(&deviceExtensionNames[0]);

            // Merge the runtime's request with the applications requests
            for (uint32_t i = 0; i < createInfo->vulkanCreateInfo->enabledExtensionCount; ++i) {
                extensions.push_back(createInfo->vulkanCreateInfo->ppEnabledExtensionNames[i]);
            }

            for (uint32_t i = 0; i < extensions.size(); i++) {
                TraceLoggingWrite(g_traceProvider, "xrCreateVulkanDeviceKHR", TLArg(extensions[i], "Extension"));
            }

            // Enable timeline semaphores.
            VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures{
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES};
            timelineSemaphoreFeatures.timelineSemaphore = true;

            VkDeviceCreateInfo deviceInfo = *createInfo->vulkanCreateInfo;
            timelineSemaphoreFeatures.pNext = (void*)deviceInfo.pNext;
            deviceInfo.pNext = &timelineSemaphoreFeatures;
            deviceInfo.enabledExtensionCount = (uint32_t)extensions.size();
            deviceInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

            auto pfnCreateDevice =
                (PFN_vkCreateDevice)createInfo->pfnGetInstanceProcAddr(m_vkBootstrapInstance, "vkCreateDevice");
            *vulkanResult =
                pfnCreateDevice(m_vkBootstrapPhysicalDevice, &deviceInfo, createInfo->vulkanAllocator, vulkanDevice);

            TraceLoggingWrite(g_traceProvider,
                              "xrCreateVulkanDeviceKHR",
                              TLPArg(*vulkanDevice, "VkDevice"),
                              TLArg((int)*vulkanResult, "VkResult"));

            m_vkDispatch.vkGetInstanceProcAddr = createInfo->pfnGetInstanceProcAddr;
            m_vkAllocator = createInfo->vulkanAllocator;

            return XR_SUCCESS;
        }

        // XR_KHR_vulkan_enable2
        // This wrapper is adapted from Khronos SDK's Vulkan plugin.
        XrResult xrGetVulkanGraphicsDevice2KHR(XrInstance instance,
                                               const XrVulkanGraphicsDeviceGetInfoKHR* getInfo,
                                               VkPhysicalDevice* vulkanPhysicalDevice) override {
            if (getInfo->type != XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrGetVulkanGraphicsDevice2KHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)getInfo->systemId, "SystemId"),
                              TLPArg(getInfo->vulkanInstance, "VkInstance"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(getInfo->systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            CHECK_XRCMD(xrGetVulkanGraphicsDeviceKHR(
                instance, getInfo->systemId, getInfo->vulkanInstance, vulkanPhysicalDevice));

            TraceLoggingWrite(
                g_traceProvider, "xrGetVulkanGraphicsDevice2KHR", TLPArg(*vulkanPhysicalDevice, "VkPhysicalDevice"));

            m_vkBootstrapPhysicalDevice = *vulkanPhysicalDevice;

            return XR_SUCCESS;
        }

        // XR_KHR_vulkan_enable
        XrResult xrGetVulkanGraphicsRequirementsKHR(XrInstance instance,
                                                    XrSystemId systemId,
                                                    XrGraphicsRequirementsVulkanKHR* graphicsRequirements) override {
            if (graphicsRequirements->type != XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrGetVulkanGraphicsRequirementsKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)systemId, "SystemId"));

            if (!m_isVulkanEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            // Require Vulkan 1.1 at minimum.
            // See https://github.com/mbucchia/OpenXR-Vk-D3D12/issues/3
            graphicsRequirements->minApiVersionSupported = XR_MAKE_VERSION(1, 1, 0);
            graphicsRequirements->maxApiVersionSupported = XR_MAKE_VERSION(2, 0, 0);

            m_graphicsRequirementQueried = true;

            TraceLoggingWrite(
                g_traceProvider,
                "xrGetVulkanGraphicsRequirementsKHR",
                TLArg(xr::ToString(graphicsRequirements->minApiVersionSupported).c_str(), "MinApiVersionSupported"),
                TLArg(xr::ToString(graphicsRequirements->maxApiVersionSupported).c_str(), "MaxApiVersionSupported"));

            return XR_SUCCESS;
        }

        // XR_KHR_vulkan_enable2
        XrResult xrGetVulkanGraphicsRequirements2KHR(XrInstance instance,
                                                     XrSystemId systemId,
                                                     XrGraphicsRequirementsVulkanKHR* graphicsRequirements) override {
            return xrGetVulkanGraphicsRequirementsKHR(instance, systemId, graphicsRequirements);
        }

        // XR_KHR_opengl_enable
        XrResult xrGetOpenGLGraphicsRequirementsKHR(XrInstance instance,
                                                    XrSystemId systemId,
                                                    XrGraphicsRequirementsOpenGLKHR* graphicsRequirements) override {
            if (graphicsRequirements->type != XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrGetOpenGLGraphicsRequirementsKHR",
                              TLPArg(instance, "Instance"),
                              TLArg((int)systemId, "SystemId"));

            if (!m_isOpenGLEnabled) {
                return XR_ERROR_FUNCTION_UNSUPPORTED;
            }

            if (!isSystemHandled(systemId)) {
                return XR_ERROR_SYSTEM_INVALID;
            }

            // TODO: The correct version to expect is 4.5, but certain applications will fail with it.
            graphicsRequirements->minApiVersionSupported = XR_MAKE_VERSION(4, 0, 0);
            graphicsRequirements->maxApiVersionSupported = XR_MAKE_VERSION(5, 0, 0);

            m_graphicsRequirementQueried = true;

            TraceLoggingWrite(
                g_traceProvider,
                "xrGetOpenGLGraphicsRequirementsKHR",
                TLArg(xr::ToString(graphicsRequirements->minApiVersionSupported).c_str(), "MinApiVersionSupported"),
                TLArg(xr::ToString(graphicsRequirements->maxApiVersionSupported).c_str(), "MaxApiVersionSupported"));

            return XR_SUCCESS;
        }

        XrResult xrEnumerateSwapchainFormats(XrSession session,
                                             uint32_t formatCapacityInput,
                                             uint32_t* formatCountOutput,
                                             int64_t* formats) override {
            TraceLoggingWrite(g_traceProvider,
                              "xrEnumerateSwapchainFormats",
                              TLPArg(session, "Session"),
                              TLArg(formatCapacityInput, "FormatCapacityInput"));

            const XrResult result =
                OpenXrApi::xrEnumerateSwapchainFormats(session, formatCapacityInput, formatCountOutput, formats);

            if (XR_SUCCEEDED(result)) {
                TraceLoggingWrite(
                    g_traceProvider, "xrEnumerateSwapchainFormats", TLArg(*formatCountOutput, "FormatCountOutput"));
                if (isSessionHandled(session) && formatCapacityInput) {
                    auto& sessionState = m_sessions[session];

                    // Translate supported formats.
                    // TODO: Better handle when a runtime format is not available. For now we just copy an adjacent
                    // format (duplicate).
                    std::optional<int64_t> firstAvailableFormat;
                    uint32_t firstAvailableFormatIndex = 0;
#define TRANSLATE_FORMAT(table, type)                                                                                  \
    {                                                                                                                  \
        for (uint32_t i = 0; i < *formatCountOutput; i++) {                                                            \
            bool found = false;                                                                                        \
            for (size_t j = 0; j < ARRAYSIZE(table); j++) {                                                            \
                if ((int64_t)table[j].dxgi == formats[i]) {                                                            \
                    formats[i] = (int64_t)table[j].type;                                                               \
                    if (!firstAvailableFormat) {                                                                       \
                        firstAvailableFormatIndex = i;                                                                 \
                        firstAvailableFormat = formats[i];                                                             \
                    }                                                                                                  \
                    TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainFormats", TLArg(formats[i], "Format"));    \
                    found = true;                                                                                      \
                    break;                                                                                             \
                }                                                                                                      \
            }                                                                                                          \
            if (!found && firstAvailableFormat) {                                                                      \
                formats[i] = firstAvailableFormat.value();                                                             \
            }                                                                                                          \
        }                                                                                                              \
        if (firstAvailableFormat) {                                                                                    \
            for (uint32_t i = 0; i < firstAvailableFormatIndex; i++) {                                                 \
                formats[i] = firstAvailableFormat.value();                                                             \
            }                                                                                                          \
        }                                                                                                              \
    }

                    if (sessionState.api == GfxApi::Vulkan) {
                        TRANSLATE_FORMAT(util::DxgiToVkFormat, vk);
                    } else {
                        TRANSLATE_FORMAT(util::DxgiToGlFormat, gl);
                    }
#undef TRANSLATE_FORMAT
                }
            }

            return result;
        }

        XrResult xrCreateSession(XrInstance instance,
                                 const XrSessionCreateInfo* createInfo,
                                 XrSession* session) override {
            if (createInfo->type != XR_TYPE_SESSION_CREATE_INFO) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrCreateSession",
                              TLPArg(instance, "Instance"),
                              TLArg((int)createInfo->systemId, "SystemId"),
                              TLArg(createInfo->createFlags, "CreateFlags"));

            XrGraphicsBindingD3D12KHR d3dBindings{XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};
            Session newSession;
            newSession.xrInstance = instance;
            bool handled = false;

            if (isSystemHandled(createInfo->systemId)) {
                const XrBaseInStructure* const* pprev =
                    reinterpret_cast<const XrBaseInStructure* const*>(&createInfo->next);
                const XrBaseInStructure* entry = reinterpret_cast<const XrBaseInStructure*>(createInfo->next);
                while (entry) {
                    const bool isVulkan = m_isVulkanEnabled && entry->type == XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
                    const bool isOpenGL = m_isOpenGLEnabled && entry->type == XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR;

                    if (isVulkan || isOpenGL) {
                        if (!m_graphicsRequirementQueried) {
                            return XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING;
                        }

                        // Create D3D12 resources.
                        {
                            ComPtr<IDXGIFactory1> dxgiFactory;
                            CHECK_HRCMD(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf())));

                            ComPtr<IDXGIAdapter1> dxgiAdapter;
                            for (UINT adapterIndex = 0;; adapterIndex++) {
                                // EnumAdapters1 will fail with DXGI_ERROR_NOT_FOUND when there are no more adapters to
                                // enumerate.
                                CHECK_HRCMD(
                                    dxgiFactory->EnumAdapters1(adapterIndex, dxgiAdapter.ReleaseAndGetAddressOf()));

                                DXGI_ADAPTER_DESC1 adapterDesc;
                                CHECK_HRCMD(dxgiAdapter->GetDesc1(&adapterDesc));
                                if (!memcmp(&adapterDesc.AdapterLuid, &m_d3d12Requirements.adapterLuid, sizeof(LUID))) {
                                    const std::wstring wadapterDescription(adapterDesc.Description);
                                    std::string adapterDescription;
                                    std::transform(wadapterDescription.begin(),
                                                   wadapterDescription.end(),
                                                   std::back_inserter(adapterDescription),
                                                   [](wchar_t c) { return (char)c; });

                                    TraceLoggingWrite(g_traceProvider,
                                                      "xrCreateSession",
                                                      TLArg(adapterDescription.c_str(), "DeviceName"));
                                    Log("Using Direct3D 12 on adapter: %s\n", adapterDescription.c_str());
                                    break;
                                }
                            }

                            // Create the interop device that the runtime will be using...
                            CHECK_HRCMD(D3D12CreateDevice(dxgiAdapter.Get(),
                                                          m_d3d12Requirements.minFeatureLevel,
                                                          IID_PPV_ARGS(newSession.d3dDevice.ReleaseAndGetAddressOf())));

                            // ... and the necessary queue.
                            D3D12_COMMAND_QUEUE_DESC queueDesc;
                            ZeroMemory(&queueDesc, sizeof(queueDesc));
                            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                            CHECK_HRCMD(newSession.d3dDevice->CreateCommandQueue(
                                &queueDesc, IID_PPV_ARGS(newSession.d3dQueue.ReleaseAndGetAddressOf())));

                            // We will use a shareable fence to synchronize between the Vulkan queue and the D3D queue.
                            CHECK_HRCMD(newSession.d3dDevice->CreateFence(
                                0,
                                D3D12_FENCE_FLAG_SHARED,
                                IID_PPV_ARGS(newSession.d3dFence.ReleaseAndGetAddressOf())));
                        }

                        // Create Vulkan resources.
                        if (isVulkan) {
                            const XrGraphicsBindingVulkanKHR* vkBindings =
                                reinterpret_cast<const XrGraphicsBindingVulkanKHR*>(entry);

                            TraceLoggingWrite(g_traceProvider, "xrCreateSession", TLArg("Vulkan", "Api"));
                            Log("Using Vulkan interop\n");

                            if (vkBindings->instance == VK_NULL_HANDLE || vkBindings->device == VK_NULL_HANDLE ||
                                vkBindings->physicalDevice == VK_NULL_HANDLE) {
                                return XR_ERROR_GRAPHICS_DEVICE_INVALID;
                            }

                            // Gather function pointers for the Vulkan device extensions we are going to use.
                            initializeVulkanDispatch(vkBindings->instance);

                            newSession.vkInstance = vkBindings->instance;
                            newSession.vkDevice = vkBindings->device;
                            newSession.vkPhysicalDevice = vkBindings->physicalDevice;

                            // Check that the app is using the correct adapter.
                            VkPhysicalDeviceIDProperties deviceId{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
                            VkPhysicalDeviceProperties2 properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                                                                   &deviceId};
                            m_vkDispatch.vkGetPhysicalDeviceProperties2(newSession.vkPhysicalDevice, &properties);
                            if (!deviceId.deviceLUIDValid ||
                                memcmp(&m_d3d12Requirements.adapterLuid, deviceId.deviceLUID, sizeof(LUID))) {
                                Log("Application did not initialize the correct adapter\n");
                                return XR_ERROR_GRAPHICS_DEVICE_INVALID;
                            }

                            m_vkDispatch.vkGetPhysicalDeviceMemoryProperties(newSession.vkPhysicalDevice,
                                                                             &newSession.memoryProperties);

                            m_vkDispatch.vkGetDeviceQueue(newSession.vkDevice,
                                                          vkBindings->queueFamilyIndex,
                                                          vkBindings->queueIndex,
                                                          &newSession.vkQueue);

                            // Create the timeline semaphore that we will use to synchronize between the Vulkan
                            // queue and the D3D queue.
                            VkSemaphoreTypeCreateInfo timelineCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};
                            timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
                            VkSemaphoreCreateInfo createInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                             &timelineCreateInfo};
                            CHECK_VKCMD(m_vkDispatch.vkCreateSemaphore(
                                newSession.vkDevice, &createInfo, m_vkAllocator, &newSession.vkTimelineSemaphore));

                            // Import the D3D fence into the semaphore.
                            wil::unique_handle fenceHandle = nullptr;
                            CHECK_HRCMD(newSession.d3dDevice->CreateSharedHandle(
                                newSession.d3dFence.Get(), nullptr, GENERIC_ALL, nullptr, fenceHandle.put()));

                            VkImportSemaphoreWin32HandleInfoKHR semaphoreImportInfo{
                                VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR};
                            semaphoreImportInfo.semaphore = newSession.vkTimelineSemaphore;
                            semaphoreImportInfo.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE_BIT;
                            semaphoreImportInfo.handle = fenceHandle.get();
                            CHECK_VKCMD(m_vkDispatch.vkImportSemaphoreWin32HandleKHR(newSession.vkDevice,
                                                                                     &semaphoreImportInfo));

                            // Create a command buffer for transitioning the layout in xrCreateSwapchain().
                            VkCommandPoolCreateInfo poolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
                            poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                            poolCreateInfo.queueFamilyIndex = vkBindings->queueFamilyIndex;
                            CHECK_VKCMD(m_vkDispatch.vkCreateCommandPool(
                                newSession.vkDevice, &poolCreateInfo, m_vkAllocator, &newSession.vkCmdPool));

                            VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
                            allocateInfo.commandPool = newSession.vkCmdPool;
                            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                            allocateInfo.commandBufferCount = 1;

                            CHECK_VKCMD(m_vkDispatch.vkAllocateCommandBuffers(
                                newSession.vkDevice, &allocateInfo, &newSession.vkCmdBuffer));

                            newSession.api = GfxApi::Vulkan;
                        } else {
                            const XrGraphicsBindingOpenGLWin32KHR* glBindings =
                                reinterpret_cast<const XrGraphicsBindingOpenGLWin32KHR*>(entry);

                            TraceLoggingWrite(g_traceProvider, "xrCreateSession", TLArg("OpenGL", "Api"));
                            Log("Using OpenGL interop\n");

                            if (!glBindings->hDC || !glBindings->hGLRC) {
                                return XR_ERROR_GRAPHICS_DEVICE_INVALID;
                            }

                            // Gather function pointers for the Vulkan device extensions we are going to use.
                            initializeOpenGLDispatch();

                            newSession.glDC = glBindings->hDC;
                            newSession.glRC = glBindings->hGLRC;

                            GlContextSwitch context(newSession);

                            // Check that the app is using the correct adapter.
                            LUID adapterLuid{};
                            m_glDispatch.glGetUnsignedBytevEXT(GL_DEVICE_LUID_EXT, (GLubyte*)&adapterLuid);
                            if (memcmp(&adapterLuid, &m_d3d12Requirements.adapterLuid, sizeof(LUID))) {
                                Log("Application did not initialize the correct adapter\n");
                                return XR_ERROR_GRAPHICS_DEVICE_INVALID;
                            }

                            // Create the semaphore that we will use to synchronize between the OpenGL context
                            // and the D3D queue.
                            m_glDispatch.glGenSemaphoresEXT(1, &newSession.glSemaphore);

                            // Import the D3D fence into the semaphore.
                            CHECK_HRCMD(
                                newSession.d3dDevice->CreateSharedHandle(newSession.d3dFence.Get(),
                                                                         nullptr,
                                                                         GENERIC_ALL,
                                                                         nullptr,
                                                                         newSession.fenceHandleForAMDWorkaround.put()));

                            m_glDispatch.glImportSemaphoreWin32HandleEXT(newSession.glSemaphore,
                                                                         GL_HANDLE_TYPE_D3D12_FENCE_EXT,
                                                                         newSession.fenceHandleForAMDWorkaround.get());

                            newSession.api = GfxApi::OpenGL;
                        }

                        // Fill out the struct that we are passing to the OpenXR runtime.
                        // TODO: Do not write to the const struct!
                        *const_cast<XrBaseInStructure**>(pprev) = reinterpret_cast<XrBaseInStructure*>(&d3dBindings);
                        d3dBindings.next = entry->next;
                        d3dBindings.device = newSession.d3dDevice.Get();
                        d3dBindings.queue = newSession.d3dQueue.Get();

                        handled = true;

                        break;
                    }

                    entry = entry->next;
                }
            }

            const XrResult result = OpenXrApi::xrCreateSession(instance, createInfo, session);
            if (handled) {
                if (XR_SUCCEEDED(result)) {
                    // On success, record the state.
                    newSession.xrSession = *session;
                    m_sessions.insert_or_assign(*session, newSession);
                } else {
                    // Cleanup on error.
                    if (newSession.vkCmdBuffer != VK_NULL_HANDLE) {
                        m_vkDispatch.vkFreeCommandBuffers(
                            newSession.vkDevice, newSession.vkCmdPool, 1, &newSession.vkCmdBuffer);
                    }
                    if (newSession.vkCmdPool != VK_NULL_HANDLE) {
                        m_vkDispatch.vkDestroyCommandPool(newSession.vkDevice, newSession.vkCmdPool, m_vkAllocator);
                    }
                    if (newSession.vkDevice != VK_NULL_HANDLE) {
                        m_vkDispatch.vkDestroySemaphore(
                            newSession.vkDevice, newSession.vkTimelineSemaphore, m_vkAllocator);
                    }
                    if (newSession.glSemaphore) {
                        GlContextSwitch context(newSession);

                        m_glDispatch.glDeleteSemaphoresEXT(1, &newSession.glSemaphore);
                    }
                }
            }

            if (XR_SUCCEEDED(result)) {
                TraceLoggingWrite(g_traceProvider, "xrCreateSession", TLPArg(*session, "Session"));
            }

            return result;
        }

        XrResult xrDestroySession(XrSession session) override {
            TraceLoggingWrite(g_traceProvider, "xrDestroySession", TLPArg(session, "Session"));

            const XrResult result = OpenXrApi::xrDestroySession(session);
            if (XR_SUCCEEDED(result) && isSessionHandled(session)) {
                auto& sessionState = m_sessions[session];

                cleanupSession(sessionState);
                m_sessions.erase(session);
            }

            return result;
        }

        XrResult xrBeginSession(XrSession session, const XrSessionBeginInfo* beginInfo) override {
            if (beginInfo->type != XR_TYPE_SESSION_BEGIN_INFO) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(
                g_traceProvider,
                "xrBeginSession",
                TLPArg(session, "Session"),
                TLArg(xr::ToCString(beginInfo->primaryViewConfigurationType), "PrimaryViewConfigurationType"));

            const XrResult result = OpenXrApi::xrBeginSession(session, beginInfo);
            if (XR_SUCCEEDED(result) && isSessionHandled(session)) {
                auto& sessionState = m_sessions[session];

                XrViewConfigurationProperties properties{XR_TYPE_VIEW_CONFIGURATION_PROPERTIES};
                CHECK_XRCMD(xrGetViewConfigurationProperties(
                    sessionState.xrInstance, m_systemId, beginInfo->primaryViewConfigurationType, &properties));

                sessionState.isFovMutable = properties.fovMutable;

                // TODO: For now we don't support inverting the image ourselves and rely on the FOV.
                if (sessionState.api == GfxApi::OpenGL && !sessionState.isFovMutable) {
                    Log("Runtime does not support mutable FOV, image may be upside-down!\n");
                }
            }

            return result;
        }

        XrResult xrCreateSwapchain(XrSession session,
                                   const XrSwapchainCreateInfo* createInfo,
                                   XrSwapchain* swapchain) override {
            if (createInfo->type != XR_TYPE_SWAPCHAIN_CREATE_INFO) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrCreateSwapchain",
                              TLPArg(session, "Session"),
                              TLArg(createInfo->arraySize, "ArraySize"),
                              TLArg(createInfo->width, "Width"),
                              TLArg(createInfo->height, "Height"),
                              TLArg(createInfo->createFlags, "CreateFlags"),
                              TLArg(createInfo->format, "Format"),
                              TLArg(createInfo->faceCount, "FaceCount"),
                              TLArg(createInfo->mipCount, "MipCount"),
                              TLArg(createInfo->sampleCount, "SampleCount"),
                              TLArg(createInfo->usageFlags, "UsageFlags"));

            XrSwapchainCreateInfo chainCreateInfo = *createInfo;
            Swapchain newSwapchain;
            bool handled = false;

            if (isSessionHandled(session)) {
                auto& sessionState = m_sessions[session];

                Log("Creating swapchain with dimensions=%ux%u, arraySize=%u, mipCount=%u, sampleCount=%u, "
                    "format=%d, "
                    "usage=0x%x\n",
                    createInfo->width,
                    createInfo->height,
                    createInfo->arraySize,
                    createInfo->mipCount,
                    createInfo->sampleCount,
                    createInfo->format,
                    createInfo->usageFlags);

                // Translate the format.
#define TRANSLATE_FORMAT(table, type)                                                                                  \
    for (size_t i = 0; i < ARRAYSIZE(table); i++) {                                                                    \
        if (table[i].type == createInfo->format) {                                                                     \
            chainCreateInfo.format = table[i].dxgi;                                                                    \
            break;                                                                                                     \
        }                                                                                                              \
    }

                if (sessionState.api == GfxApi::Vulkan) {
                    TRANSLATE_FORMAT(util::DxgiToVkFormat, vk);
                } else {
                    TRANSLATE_FORMAT(util::DxgiToGlFormat, gl);
                }

                Log("Translated format: %d\n", chainCreateInfo.format);

                newSwapchain.xrSession = session;
                newSwapchain.createInfo = *createInfo;

                // The rest will be filled in by xrEnumerateSwapchainImages().

                handled = true;
            }

            const XrResult result = OpenXrApi::xrCreateSwapchain(session, &chainCreateInfo, swapchain);
            if (XR_SUCCEEDED(result) && handled) {
                // On success, record the state.
                newSwapchain.xrSwapchain = *swapchain;
                m_swapchains.insert_or_assign(*swapchain, newSwapchain);
            }

            TraceLoggingWrite(g_traceProvider, "xrCreateSwapchain", TLPArg(*swapchain, "Swapchain"));

            return result;
        }

        XrResult xrDestroySwapchain(XrSwapchain swapchain) override {
            TraceLoggingWrite(g_traceProvider, "xrDestroySwapchain", TLPArg(swapchain, "Swapchain"));

            const XrResult result = OpenXrApi::xrDestroySwapchain(swapchain);
            if (XR_SUCCEEDED(result) && isSwapchainHandled(swapchain)) {
                auto& swapchainState = m_swapchains[swapchain];

                cleanupSwapchain(swapchainState);
                m_swapchains.erase(swapchain);
            }

            return result;
        }

        XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain,
                                            uint32_t imageCapacityInput,
                                            uint32_t* imageCountOutput,
                                            XrSwapchainImageBaseHeader* images) override {
            TraceLoggingWrite(g_traceProvider,
                              "xrEnumerateSwapchainImages",
                              TLPArg(swapchain, "Swapchain"),
                              TLArg(imageCapacityInput, "ImageCapacityInput"));

            if (!isSwapchainHandled(swapchain) || imageCapacityInput == 0) {
                const XrResult result =
                    OpenXrApi::xrEnumerateSwapchainImages(swapchain, imageCapacityInput, imageCountOutput, images);
                TraceLoggingWrite(
                    g_traceProvider, "xrEnumerateSwapchainImages", TLArg(*imageCountOutput, "ImageCountOutput"));
                return result;
            }

            // Enumerate the actual D3D swapchain images.
            std::vector<XrSwapchainImageD3D12KHR> d3dImages(imageCapacityInput, {XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR});
            const XrResult result =
                OpenXrApi::xrEnumerateSwapchainImages(swapchain,
                                                      imageCapacityInput,
                                                      imageCountOutput,
                                                      reinterpret_cast<XrSwapchainImageBaseHeader*>(d3dImages.data()));
            if (XR_SUCCEEDED(result)) {
                TraceLoggingWrite(
                    g_traceProvider, "xrEnumerateSwapchainImages", TLArg(*imageCountOutput, "ImageCountOutput"));

                auto& swapchainState = m_swapchains[swapchain];
                auto& sessionState = m_sessions[swapchainState.xrSession];

                const bool isVulkan = sessionState.api == GfxApi::Vulkan;

                const bool initialized = !swapchainState.vkImage.empty() || !swapchainState.glImage.empty();

                // Helper to select the memory type.
                auto findMemoryType = [sessionState](uint32_t memoryTypeBitsRequirement, VkFlags requirementsMask) {
                    for (uint32_t memoryIndex = 0; memoryIndex < VK_MAX_MEMORY_TYPES; ++memoryIndex) {
                        const uint32_t memoryTypeBits = (1 << memoryIndex);
                        const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
                        const bool satisfiesFlags =
                            (sessionState.memoryProperties.memoryTypes[memoryIndex].propertyFlags & requirementsMask) ==
                            requirementsMask;

                        if (isRequiredMemoryType && satisfiesFlags) {
                            return memoryIndex;
                        }
                    }

                    CHECK_VKCMD(VK_ERROR_UNKNOWN);
                    return 0u;
                };

                // Start a command list to transition images.
                if (isVulkan &&
                    (swapchainState.createInfo.usageFlags &
                     (XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))) {
                    // Simplify our code here by waiting for the device to be idle.
                    m_vkDispatch.vkDeviceWaitIdle(sessionState.vkDevice);

                    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
                    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                    CHECK_VKCMD(m_vkDispatch.vkBeginCommandBuffer(sessionState.vkCmdBuffer, &beginInfo));
                }

                // Export each D3D12 texture to Vulkan/OpenGL.
                for (uint32_t i = 0; i < *imageCountOutput; i++) {
                    if (!initialized) {
                        // Dump the runtime texture descriptor.
                        if (i == 0) {
                            const auto& desc = d3dImages[0].texture->GetDesc();
                            TraceLoggingWrite(g_traceProvider,
                                              "xrEnumerateSwapchainImages",
                                              TLArg("D3D12", "Api"),
                                              TLArg(desc.Width, "Width"),
                                              TLArg(desc.Height, "Height"),
                                              TLArg(desc.DepthOrArraySize, "ArraySize"),
                                              TLArg(desc.MipLevels, "MipCount"),
                                              TLArg(desc.SampleDesc.Count, "SampleCount"),
                                              TLArg((int)desc.Format, "Format"),
                                              TLArg((int)desc.Flags, "Flags"));
                            Log("Swapchain image descriptor:\n");
                            Log("  w=%u h=%u arraySize=%u format=%u\n",
                                desc.Width,
                                desc.Height,
                                desc.DepthOrArraySize,
                                desc.Format);
                            Log("  mipCount=%u sampleCount=%u\n", desc.MipLevels, desc.SampleDesc.Count);
                            Log("  flags=0x%x\n", desc.Flags);
                        }

                        wil::unique_handle textureHandle = nullptr;
                        CHECK_HRCMD(sessionState.d3dDevice->CreateSharedHandle(
                            d3dImages[i].texture, nullptr, GENERIC_ALL, nullptr, textureHandle.put()));

                        if (isVulkan) {
                            // Prepare the Vulkan image that the app will use.
                            VkImage image;

                            VkExternalMemoryImageCreateInfo externalCreateInfo{
                                VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO};
                            externalCreateInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT;

                            VkImageCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, &externalCreateInfo};
                            createInfo.imageType = VK_IMAGE_TYPE_2D;
                            createInfo.format = (VkFormat)swapchainState.createInfo.format;
                            createInfo.extent.width = swapchainState.createInfo.width;
                            createInfo.extent.height = swapchainState.createInfo.height;
                            createInfo.extent.depth = 1;
                            createInfo.mipLevels = swapchainState.createInfo.mipCount;
                            createInfo.arrayLayers = swapchainState.createInfo.arraySize;
                            createInfo.samples = (VkSampleCountFlagBits)swapchainState.createInfo.sampleCount;
                            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                            createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                            if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT) {
                                createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                            }
                            if (swapchainState.createInfo.usageFlags &
                                XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                                createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                            }
                            if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_SAMPLED_BIT) {
                                createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
                            }
                            if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT) {
                                createInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
                            }
                            if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_TRANSFER_SRC_BIT) {
                                createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                            }
                            if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT) {
                                createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                            }
                            if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT) {
                                createInfo.usage |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
                            }
                            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                            CHECK_VKCMD(
                                m_vkDispatch.vkCreateImage(sessionState.vkDevice, &createInfo, m_vkAllocator, &image));

                            swapchainState.vkImage.push_back(image);

                            // Import the device memory from D3D.
                            VkDeviceMemory memory;

                            VkImageMemoryRequirementsInfo2 requirementInfo{
                                VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2};
                            requirementInfo.image = image;
                            VkMemoryRequirements2 requirements{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
                            m_vkDispatch.vkGetImageMemoryRequirements2KHR(
                                sessionState.vkDevice, &requirementInfo, &requirements);

                            VkMemoryWin32HandlePropertiesKHR handleProperties{
                                VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR};
                            CHECK_VKCMD(m_vkDispatch.vkGetMemoryWin32HandlePropertiesKHR(
                                sessionState.vkDevice,
                                VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT,
                                textureHandle.get(),
                                &handleProperties));

                            VkImportMemoryWin32HandleInfoKHR importInfo{
                                VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR};
                            importInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT;
                            importInfo.handle = textureHandle.get();

                            VkMemoryDedicatedAllocateInfo memoryAllocateInfo{
                                VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO, &importInfo};
                            memoryAllocateInfo.image = image;

                            VkMemoryAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                              &memoryAllocateInfo};
                            allocateInfo.allocationSize = requirements.memoryRequirements.size;
                            allocateInfo.memoryTypeIndex =
                                findMemoryType(handleProperties.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),

                            CHECK_VKCMD(m_vkDispatch.vkAllocateMemory(
                                sessionState.vkDevice, &allocateInfo, m_vkAllocator, &memory));

                            swapchainState.vkDeviceMemory.push_back(memory);

                            VkBindImageMemoryInfo bindImageInfo{VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO};
                            bindImageInfo.image = image;
                            bindImageInfo.memory = memory;
                            CHECK_VKCMD(m_vkDispatch.vkBindImageMemory2KHR(sessionState.vkDevice, 1, &bindImageInfo));

                            // Transition the image to the layout expected by the application.
                            if (swapchainState.createInfo.usageFlags &
                                (XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT |
                                 XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
                                VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
                                barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                if (swapchainState.createInfo.usageFlags & XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT) {
                                    barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                                }
                                if (swapchainState.createInfo.usageFlags &
                                    XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                                    barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                                }
                                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                                barrier.image = swapchainState.vkImage[i];
                                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                barrier.subresourceRange.baseMipLevel = 0;
                                barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
                                barrier.subresourceRange.baseArrayLayer = 0;
                                barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

                                m_vkDispatch.vkCmdPipelineBarrier(sessionState.vkCmdBuffer,
                                                                  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                                  VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                                                  0,
                                                                  0,
                                                                  (VkMemoryBarrier*)nullptr,
                                                                  0,
                                                                  (VkBufferMemoryBarrier*)nullptr,
                                                                  1,
                                                                  &barrier);
                            }
                        } else {
                            GlContextSwitch context(sessionState);

                            const auto& createInfo = swapchainState.createInfo;

                            // Import the device memory from D3D.
                            GLuint memory;
                            m_glDispatch.glCreateMemoryObjectsEXT(1, &memory);
                            swapchainState.glMemory.push_back(memory);

                            size_t bytePerPixels = 0;
                            for (size_t j = 0; j < ARRAYSIZE(util::DxgiToGlFormat); j++) {
                                if (util::DxgiToGlFormat[j].gl == createInfo.format) {
                                    bytePerPixels = util::DxgiToGlFormat[j].size;
                                    break;
                                }
                            }

                            // TODO: Not sure why we need to multiply by 2. Mipmapping?
                            // https://stackoverflow.com/questions/71108346/how-to-use-glimportmemorywin32handleext-to-share-an-id3d11texture2d-keyedmutex-s
                            m_glDispatch.glImportMemoryWin32HandleEXT(memory,
                                                                      createInfo.arraySize * createInfo.width *
                                                                          createInfo.height * createInfo.sampleCount *
                                                                          bytePerPixels * 2,
                                                                      GL_HANDLE_TYPE_D3D12_RESOURCE_EXT,
                                                                      textureHandle.get());

                            // Create the texture that the app will use.
                            GLuint image;

                            if (createInfo.arraySize == 1) {
                                if (createInfo.sampleCount == 1) {
                                    m_glDispatch.glCreateTextures(GL_TEXTURE_2D, 1, &image);
                                    m_glDispatch.glTextureStorageMem2DEXT(image,
                                                                          createInfo.mipCount,
                                                                          (GLenum)createInfo.format,
                                                                          createInfo.width,
                                                                          createInfo.height,
                                                                          memory,
                                                                          0);
                                } else {
                                    m_glDispatch.glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &image);
                                    m_glDispatch.glTextureStorageMem2DMultisampleEXT(image,
                                                                                     createInfo.sampleCount,
                                                                                     (GLenum)createInfo.format,
                                                                                     createInfo.width,
                                                                                     createInfo.height,
                                                                                     GL_TRUE,
                                                                                     memory,
                                                                                     0);
                                }
                            } else {
                                if (createInfo.sampleCount == 1) {
                                    m_glDispatch.glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &image);
                                    m_glDispatch.glTextureStorageMem3DEXT(image,
                                                                          createInfo.mipCount,
                                                                          (GLenum)createInfo.format,
                                                                          createInfo.width,
                                                                          createInfo.height,
                                                                          createInfo.arraySize,
                                                                          memory,
                                                                          0);
                                } else {
                                    m_glDispatch.glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 1, &image);
                                    m_glDispatch.glTextureStorageMem3DMultisampleEXT(image,
                                                                                     createInfo.sampleCount,
                                                                                     (GLenum)createInfo.format,
                                                                                     createInfo.width,
                                                                                     createInfo.height,
                                                                                     createInfo.arraySize,
                                                                                     GL_TRUE,
                                                                                     memory,
                                                                                     0);
                                }
                            }
                            swapchainState.glImage.push_back(image);
                        }
                    }

                    if (isVulkan) {
                        XrSwapchainImageVulkanKHR* vkImages = reinterpret_cast<XrSwapchainImageVulkanKHR*>(images);
                        vkImages[i].image = swapchainState.vkImage[i];

                        TraceLoggingWrite(g_traceProvider,
                                          "xrEnumerateSwapchainImages",
                                          TLArg("Vulkan", "Api"),
                                          TLPArg(vkImages[i].image, "Texture"));
                    } else {
                        XrSwapchainImageOpenGLKHR* glImages = reinterpret_cast<XrSwapchainImageOpenGLKHR*>(images);
                        glImages[i].image = swapchainState.glImage[i];

                        TraceLoggingWrite(g_traceProvider,
                                          "xrEnumerateSwapchainImages",
                                          TLArg("OpenGL", "Api"),
                                          TLArg(glImages[i].image, "Texture"));
                    }
                }

                // Execute the command list to transition images.
                if (isVulkan &&
                    (swapchainState.createInfo.usageFlags &
                     (XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))) {
                    m_vkDispatch.vkEndCommandBuffer(sessionState.vkCmdBuffer);

                    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
                    submitInfo.commandBufferCount = 1;
                    submitInfo.pCommandBuffers = &sessionState.vkCmdBuffer;
                    CHECK_VKCMD(m_vkDispatch.vkQueueSubmit(sessionState.vkQueue, 1, &submitInfo, VK_NULL_HANDLE));
                }
            }

            return result;
        }

        XrResult xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo) override {
            if (frameEndInfo->type != XR_TYPE_FRAME_END_INFO) {
                return XR_ERROR_VALIDATION_FAILURE;
            }

            TraceLoggingWrite(g_traceProvider,
                              "xrEndFrame",
                              TLPArg(session, "Session"),
                              TLArg(frameEndInfo->displayTime, "DisplayTime"),
                              TLArg(xr::ToCString(frameEndInfo->environmentBlendMode), "EnvironmentBlendMode"));

            // Because the frame info is passed const, we are going to need to reconstruct a writable version of it
            // to patch the FOV and invert the image with OpenGL.
            XrFrameEndInfo chainFrameEndInfo = *frameEndInfo;
            std::vector<const XrCompositionLayerBaseHeader*> correctedLayers;

            std::vector<XrCompositionLayerProjection> layerProjectionAllocator;
            std::vector<std::array<XrCompositionLayerProjectionView, 2>> layerProjectionViewsAllocator;

            if (isSessionHandled(session)) {
                auto& sessionState = m_sessions[session];

                // Signal the semaphore from the Vulkan queue/OpenGL context, and wait for it on the D3D12 queue.
                // This effectively serializes the app work between Vulkan/OpenGL and D3D12.
                sessionState.fenceValue++;
                TraceLoggingWrite(g_traceProvider, "xrEndFrame_Sync", TLArg(sessionState.fenceValue, "FenceValue"));
                if (sessionState.api == GfxApi::Vulkan) {
                    VkTimelineSemaphoreSubmitInfo timelineInfo{VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO};
                    timelineInfo.signalSemaphoreValueCount = 1;
                    timelineInfo.pSignalSemaphoreValues = &sessionState.fenceValue;
                    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO, &timelineInfo};
                    submitInfo.signalSemaphoreCount = 1;
                    submitInfo.pSignalSemaphores = &sessionState.vkTimelineSemaphore;
                    CHECK_VKCMD(m_vkDispatch.vkQueueSubmit(sessionState.vkQueue, 1, &submitInfo, VK_NULL_HANDLE));
                } else {
                    GlContextSwitch context(sessionState);

                    m_glDispatch.glSemaphoreParameterui64vEXT(
                        sessionState.glSemaphore, GL_D3D12_FENCE_VALUE_EXT, &sessionState.fenceValue);

                    m_glDispatch.glSignalSemaphoreEXT(sessionState.glSemaphore, 0, nullptr, 0, nullptr, nullptr);

                    glFlush();
                }
                CHECK_HRCMD(sessionState.d3dQueue->Wait(sessionState.d3dFence.Get(), sessionState.fenceValue));

                // When using OpenGL, the Y-axis is inverted, and we must tell the runtime to render the image
                // upside-up. We use the FOV to do that.
                if (sessionState.api == GfxApi::OpenGL && sessionState.isFovMutable) {
                    // We must reserve the underlying storage to keep our pointers stable.
                    layerProjectionAllocator.reserve(chainFrameEndInfo.layerCount);
                    layerProjectionViewsAllocator.reserve(chainFrameEndInfo.layerCount);

                    for (uint32_t i = 0; i < chainFrameEndInfo.layerCount; i++) {
                        if (chainFrameEndInfo.layers[i]->type == XR_TYPE_COMPOSITION_LAYER_PROJECTION) {
                            const XrCompositionLayerProjection* proj =
                                reinterpret_cast<const XrCompositionLayerProjection*>(chainFrameEndInfo.layers[i]);

                            auto correctedProjectionLayer = &layerProjectionAllocator.emplace_back(*proj);
                            auto correctedProjectionViews =
                                layerProjectionViewsAllocator
                                    .emplace_back(std::array<XrCompositionLayerProjectionView, 2>(
                                        {proj->views[0], proj->views[1]}))
                                    .data();

                            for (uint32_t eye = 0; eye < xr::StereoView::Count; eye++) {
                                const XrCompositionLayerProjectionView& view = proj->views[eye];

                                std::swap(correctedProjectionViews[eye].fov.angleDown,
                                          correctedProjectionViews[eye].fov.angleUp);
                            }

                            correctedProjectionLayer->views = correctedProjectionViews;
                            correctedLayers.push_back(
                                reinterpret_cast<const XrCompositionLayerBaseHeader*>(correctedProjectionLayer));
                        } else {
                            correctedLayers.push_back(chainFrameEndInfo.layers[i]);
                        }
                    }

                    chainFrameEndInfo.layers = correctedLayers.data();
                    chainFrameEndInfo.layerCount = (uint32_t)correctedLayers.size();
                }
            }

            return OpenXrApi::xrEndFrame(session, &chainFrameEndInfo);
        }

      private:
        // Initialize the function pointers for the Vulkan instance.
        void initializeVulkanDispatch(VkInstance instance) {
            PFN_vkGetInstanceProcAddr getProcAddr =
                m_vkDispatch.vkGetInstanceProcAddr ? m_vkDispatch.vkGetInstanceProcAddr : vkGetInstanceProcAddr;

#define VK_GET_PTR(fun) m_vkDispatch.fun = reinterpret_cast<PFN_##fun>(getProcAddr(instance, #fun));

            VK_GET_PTR(vkGetPhysicalDeviceProperties2);
            VK_GET_PTR(vkGetPhysicalDeviceMemoryProperties);
            VK_GET_PTR(vkGetImageMemoryRequirements2KHR);
            VK_GET_PTR(vkGetDeviceQueue);
            VK_GET_PTR(vkQueueSubmit);
            VK_GET_PTR(vkCreateImage);
            VK_GET_PTR(vkDestroyImage);
            VK_GET_PTR(vkAllocateMemory);
            VK_GET_PTR(vkFreeMemory);
            VK_GET_PTR(vkCreateCommandPool);
            VK_GET_PTR(vkDestroyCommandPool);
            VK_GET_PTR(vkAllocateCommandBuffers);
            VK_GET_PTR(vkFreeCommandBuffers);
            VK_GET_PTR(vkBeginCommandBuffer);
            VK_GET_PTR(vkCmdPipelineBarrier);
            VK_GET_PTR(vkEndCommandBuffer);
            VK_GET_PTR(vkGetMemoryWin32HandlePropertiesKHR);
            VK_GET_PTR(vkBindImageMemory2KHR);
            VK_GET_PTR(vkCreateSemaphore);
            VK_GET_PTR(vkDestroySemaphore);
            VK_GET_PTR(vkImportSemaphoreWin32HandleKHR);
            VK_GET_PTR(vkDeviceWaitIdle);

#undef VK_GET_PTR
        }

        void initializeOpenGLDispatch() {
#define GL_GET_PTR(fun)                                                                                                \
    m_glDispatch.fun = reinterpret_cast<decltype(m_glDispatch.fun)>(wglGetProcAddress(#fun));                          \
    CHECK_MSG(m_glDispatch.fun, "OpenGL driver does not support " #fun);

            GL_GET_PTR(glGetUnsignedBytevEXT);
            GL_GET_PTR(glCreateTextures);
            GL_GET_PTR(glCreateMemoryObjectsEXT);
            GL_GET_PTR(glDeleteMemoryObjectsEXT);
            GL_GET_PTR(glTextureStorageMem2DEXT);
            GL_GET_PTR(glTextureStorageMem2DMultisampleEXT);
            GL_GET_PTR(glTextureStorageMem3DEXT);
            GL_GET_PTR(glTextureStorageMem3DMultisampleEXT);
            GL_GET_PTR(glGenSemaphoresEXT);
            GL_GET_PTR(glDeleteSemaphoresEXT);
            GL_GET_PTR(glSemaphoreParameterui64vEXT);
            GL_GET_PTR(glSignalSemaphoreEXT);
            GL_GET_PTR(glImportMemoryWin32HandleEXT);
            GL_GET_PTR(glImportSemaphoreWin32HandleEXT);

#undef GL_GET_PTR
        }

        void cleanupSession(Session& sessionState) {
            // Wait for both devices to be idle.
            wil::unique_handle eventHandle;
            sessionState.d3dQueue->Signal(sessionState.d3dFence.Get(), ++sessionState.fenceValue);
            *eventHandle.put() = CreateEventEx(nullptr, L"Flush Fence", 0, EVENT_ALL_ACCESS);
            CHECK_HRCMD(sessionState.d3dFence->SetEventOnCompletion(sessionState.fenceValue, eventHandle.get()));
            WaitForSingleObject(eventHandle.get(), INFINITE);

            if (sessionState.api == GfxApi::Vulkan) {
                m_vkDispatch.vkDeviceWaitIdle(sessionState.vkDevice);
            }

            for (auto it = m_swapchains.begin(); it != m_swapchains.end();) {
                auto& swapchainState = it->second;
                if (swapchainState.xrSession == sessionState.xrSession) {
                    cleanupSwapchain(swapchainState);
                    it = m_swapchains.erase(it);
                } else {
                    it++;
                }
            }

            if (sessionState.api == GfxApi::Vulkan) {
                m_vkDispatch.vkFreeCommandBuffers(
                    sessionState.vkDevice, sessionState.vkCmdPool, 1, &sessionState.vkCmdBuffer);
                m_vkDispatch.vkDestroyCommandPool(sessionState.vkDevice, sessionState.vkCmdPool, m_vkAllocator);
                m_vkDispatch.vkDestroySemaphore(sessionState.vkDevice, sessionState.vkTimelineSemaphore, m_vkAllocator);
            } else {
                GlContextSwitch context(sessionState);

                m_glDispatch.glDeleteSemaphoresEXT(1, &sessionState.glSemaphore);
            }

            ZeroMemory(&m_vkDispatch, sizeof(m_vkDispatch));
            m_vkAllocator = nullptr;
            ZeroMemory(&m_glDispatch, sizeof(m_glDispatch));
        }

        void cleanupSwapchain(Swapchain& swapchainState) {
            auto& sessionState = m_sessions[swapchainState.xrSession];

            if (sessionState.api == GfxApi::Vulkan) {
                m_vkDispatch.vkDeviceWaitIdle(sessionState.vkDevice);
            }

            for (auto& image : swapchainState.vkImage) {
                m_vkDispatch.vkDestroyImage(sessionState.vkDevice, image, m_vkAllocator);
            }
            for (auto& memory : swapchainState.vkDeviceMemory) {
                m_vkDispatch.vkFreeMemory(sessionState.vkDevice, memory, m_vkAllocator);
            }

            GlContextSwitch context(sessionState);

            for (auto& image : swapchainState.glImage) {
                glDeleteTextures(1, &image);
            }
            for (auto& memory : swapchainState.glMemory) {
                m_glDispatch.glDeleteMemoryObjectsEXT(1, &memory);
            }
        }

        bool isSystemHandled(XrSystemId systemId) const {
            return systemId == m_systemId;
        }

        bool isSessionHandled(XrSession session) const {
            return m_sessions.find(session) != m_sessions.cend();
        }

        bool isSwapchainHandled(XrSwapchain swapchain) const {
            return m_swapchains.find(swapchain) != m_swapchains.cend();
        }

        bool m_isVulkanEnabled{false};
        bool m_isOpenGLEnabled{false};

        XrSystemId m_systemId{XR_NULL_SYSTEM_ID};
        bool m_graphicsRequirementQueried{false};
        XrGraphicsRequirementsD3D12KHR m_d3d12Requirements;

        std::map<XrSession, Session> m_sessions;
        std::map<XrSwapchain, Swapchain> m_swapchains;

        // State for XR_KHR_vulkan_enable2 emulation.
        VkInstance m_vkBootstrapInstance{VK_NULL_HANDLE};
        VkPhysicalDevice m_vkBootstrapPhysicalDevice{VK_NULL_HANDLE};

        const VkAllocationCallbacks* m_vkAllocator{nullptr};
        struct {
            PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{nullptr};

            // Pointers below must be initialized in initializeVulkanDispatch(),
            PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2{nullptr};
            PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties{nullptr};
            PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR{nullptr};
            PFN_vkGetDeviceQueue vkGetDeviceQueue{nullptr};
            PFN_vkQueueSubmit vkQueueSubmit{nullptr};
            PFN_vkCreateImage vkCreateImage{nullptr};
            PFN_vkDestroyImage vkDestroyImage{nullptr};
            PFN_vkAllocateMemory vkAllocateMemory{nullptr};
            PFN_vkFreeMemory vkFreeMemory{nullptr};
            PFN_vkCreateCommandPool vkCreateCommandPool{nullptr};
            PFN_vkDestroyCommandPool vkDestroyCommandPool{nullptr};
            PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers{nullptr};
            PFN_vkFreeCommandBuffers vkFreeCommandBuffers{nullptr};
            PFN_vkBeginCommandBuffer vkBeginCommandBuffer{nullptr};
            PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier{nullptr};
            PFN_vkEndCommandBuffer vkEndCommandBuffer{nullptr};
            PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR{nullptr};
            PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR{nullptr};
            PFN_vkCreateSemaphore vkCreateSemaphore{nullptr};
            PFN_vkDestroySemaphore vkDestroySemaphore{nullptr};
            PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR{nullptr};
            PFN_vkDeviceWaitIdle vkDeviceWaitIdle{nullptr};
        } m_vkDispatch;

        struct {
            // Pointers below must be initialized in initializeOpenGLDispatch(),
            PFNGLGETUNSIGNEDBYTEVEXTPROC glGetUnsignedBytevEXT{nullptr};
            PFNGLCREATETEXTURESPROC glCreateTextures{nullptr};
            PFNGLCREATEMEMORYOBJECTSEXTPROC glCreateMemoryObjectsEXT{nullptr};
            PFNGLDELETEMEMORYOBJECTSEXTPROC glDeleteMemoryObjectsEXT{nullptr};
            PFNGLTEXTURESTORAGEMEM2DEXTPROC glTextureStorageMem2DEXT{nullptr};
            PFNGLTEXTURESTORAGEMEM2DMULTISAMPLEEXTPROC glTextureStorageMem2DMultisampleEXT{nullptr};
            PFNGLTEXTURESTORAGEMEM3DEXTPROC glTextureStorageMem3DEXT{nullptr};
            PFNGLTEXTURESTORAGEMEM3DMULTISAMPLEEXTPROC glTextureStorageMem3DMultisampleEXT{nullptr};
            PFNGLGENSEMAPHORESEXTPROC glGenSemaphoresEXT{nullptr};
            PFNGLDELETESEMAPHORESEXTPROC glDeleteSemaphoresEXT{nullptr};
            PFNGLSEMAPHOREPARAMETERUI64VEXTPROC glSemaphoreParameterui64vEXT{nullptr};
            PFNGLSIGNALSEMAPHOREEXTPROC glSignalSemaphoreEXT{nullptr};
            PFNGLIMPORTMEMORYWIN32HANDLEEXTPROC glImportMemoryWin32HandleEXT{nullptr};
            PFNGLIMPORTSEMAPHOREWIN32HANDLEEXTPROC glImportSemaphoreWin32HandleEXT{nullptr};
        } m_glDispatch;
    };

    std::unique_ptr<OpenXrLayer> g_instance = nullptr;

} // namespace

namespace vulkan_d3d12_interop {
    OpenXrApi* GetInstance() {
        if (!g_instance) {
            g_instance = std::make_unique<OpenXrLayer>();
        }
        return g_instance.get();
    }

    void ResetInstance() {
        g_instance.reset();
    }

} // namespace vulkan_d3d12_interop

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        TraceLoggingRegister(vulkan_d3d12_interop::log::g_traceProvider);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
