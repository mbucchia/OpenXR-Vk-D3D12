// MIT License
//
// Copyright(c) 2022-2023 Matthieu Bucchianeri
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

#include "pch.h"

#include <layer.h>

#include "dispatch.h"
#include "log.h"

#ifndef LAYER_NAMESPACE
#error Must define LAYER_NAMESPACE
#endif

using namespace LAYER_NAMESPACE::log;

namespace LAYER_NAMESPACE {

    std::mutex g_bypassLock;
    std::map<XrInstance, PFN_xrGetInstanceProcAddr> g_bypass;

    // Entry point for creating the layer.
    XrResult XRAPI_CALL xrCreateApiLayerInstance(const XrInstanceCreateInfo* const instanceCreateInfo,
                                                 const struct XrApiLayerCreateInfo* const apiLayerInfo,
                                                 XrInstance* const instance) {
        TraceLoggingWrite(g_traceProvider, "xrCreateApiLayerInstance");
        DebugLog("--> xrCreateApiLayerInstance\n");

        if (!apiLayerInfo || apiLayerInfo->structType != XR_LOADER_INTERFACE_STRUCT_API_LAYER_CREATE_INFO ||
            apiLayerInfo->structVersion != XR_API_LAYER_CREATE_INFO_STRUCT_VERSION ||
            apiLayerInfo->structSize != sizeof(XrApiLayerCreateInfo) || !apiLayerInfo->nextInfo ||
            apiLayerInfo->nextInfo->structType != XR_LOADER_INTERFACE_STRUCT_API_LAYER_NEXT_INFO ||
            apiLayerInfo->nextInfo->structVersion != XR_API_LAYER_NEXT_INFO_STRUCT_VERSION ||
            apiLayerInfo->nextInfo->structSize != sizeof(XrApiLayerNextInfo) ||
            apiLayerInfo->nextInfo->layerName != LayerName || !apiLayerInfo->nextInfo->nextGetInstanceProcAddr ||
            !apiLayerInfo->nextInfo->nextCreateApiLayerInstance) {
            ErrorLog("xrCreateApiLayerInstance validation failed\n");
            return XR_ERROR_INITIALIZATION_FAILED;
        }

        // Dump the other layers.
        {
            auto info = apiLayerInfo->nextInfo;
            while (info) {
                TraceLoggingWrite(g_traceProvider, "xrCreateApiLayerInstance", TLArg(info->layerName, "LayerName"));
                Log("Using layer: %s\n", info->layerName);
                info = info->next;
            }
        }

        // See if any upstream layer or the runtime already supports Vulkan/OpenGL.
        // While the OpenXR standard states that xrEnumerateInstanceExtensionProperties() can be queried without an
        // instance, this does not stand for API layers, since API layers implementation might rely on the next
        // xrGetInstanceProcAddr() pointer, which is not (yet) populated if no instance is created.
        // We create a dummy instance in order to do these checks.
        bool need_XR_KHR_vulkan_enable = true;
        bool need_XR_KHR_vulkan_enable2 = true;
        bool need_XR_KHR_opengl_enable = true;
        {
            XrInstance dummyInstance = XR_NULL_HANDLE;

            // Call the chain to create a dummy instance. Request no extensions in order to speed things up.
            XrInstanceCreateInfo dummyCreateInfo = *instanceCreateInfo;
            dummyCreateInfo.enabledExtensionCount = 0;

            XrApiLayerCreateInfo chainApiLayerInfo = *apiLayerInfo;
            chainApiLayerInfo.nextInfo = apiLayerInfo->nextInfo->next;

            CHECK_XRCMD(apiLayerInfo->nextInfo->nextCreateApiLayerInstance(
                &dummyCreateInfo, &chainApiLayerInfo, &dummyInstance));

            // Check the available extensions.
            PFN_xrEnumerateInstanceExtensionProperties xrEnumerateInstanceExtensionProperties;
            CHECK_XRCMD(apiLayerInfo->nextInfo->nextGetInstanceProcAddr(
                dummyInstance,
                "xrEnumerateInstanceExtensionProperties",
                reinterpret_cast<PFN_xrVoidFunction*>(&xrEnumerateInstanceExtensionProperties)));

            uint32_t extensionsCount = 0;
            CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionsCount, nullptr));
            std::vector<XrExtensionProperties> extensions(extensionsCount, {XR_TYPE_EXTENSION_PROPERTIES});
            CHECK_XRCMD(
                xrEnumerateInstanceExtensionProperties(nullptr, extensionsCount, &extensionsCount, extensions.data()));

            for (uint32_t i = 0; i < extensionsCount; i++) {
                TraceLoggingWrite(g_traceProvider,
                                  "xrCreateApiLayerInstance",
                                  TLArg(extensions[i].extensionName, "AvailableExtension"));
                Log("Available extension: %s\n", extensions[i].extensionName);
                const std::string_view ext(extensions[i].extensionName);

                if (ext == XR_KHR_VULKAN_ENABLE_EXTENSION_NAME) {
                    need_XR_KHR_vulkan_enable = false;
                } else if (ext == XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME) {
                    need_XR_KHR_vulkan_enable2 = false;
                } else if (ext == XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) {
                    need_XR_KHR_opengl_enable = false;
                }
            }

            PFN_xrDestroyInstance xrDestroyInstance;
            CHECK_XRCMD(apiLayerInfo->nextInfo->nextGetInstanceProcAddr(
                dummyInstance, "xrDestroyInstance", reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyInstance)));

            CHECK_XRCMD(xrDestroyInstance(dummyInstance));
        }

        // See what extensions the application is requesting.
        bool want_XR_KHR_vulkan_enable = false;
        bool want_XR_KHR_vulkan_enable2 = false;
        bool want_XR_KHR_opengl_enable = false;
        std::vector<const char*> newEnabledExtensionNames;
        for (uint32_t i = 0; i < instanceCreateInfo->enabledExtensionCount; i++) {
            TraceLoggingWrite(g_traceProvider,
                              "xrCreateApiLayerInstance",
                              TLArg(instanceCreateInfo->enabledExtensionNames[i], "RequestedExtension"));
            Log("Requested extension: %s\n", instanceCreateInfo->enabledExtensionNames[i]);
            const std::string_view ext(instanceCreateInfo->enabledExtensionNames[i]);

            if (ext == XR_KHR_VULKAN_ENABLE_EXTENSION_NAME) {
                want_XR_KHR_vulkan_enable = true;
            } else if (ext == XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME) {
                want_XR_KHR_vulkan_enable2 = true;
            } else if (ext == XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) {
                want_XR_KHR_opengl_enable = true;
            } else {
                newEnabledExtensionNames.push_back(ext.data());
            }
        }

        // Remove the Vulkan/OpenGL extension(s) and add the D3D12 one instead, otherwise bypass entirely the layer.
        XrInstanceCreateInfo chainInstanceCreateInfo = *instanceCreateInfo;
        if ((want_XR_KHR_vulkan_enable && need_XR_KHR_vulkan_enable) ||
            (want_XR_KHR_vulkan_enable2 && need_XR_KHR_vulkan_enable2) ||
            (want_XR_KHR_opengl_enable && need_XR_KHR_opengl_enable)) {
            TraceLoggingWrite(g_traceProvider, "xrCreateApiLayerInstance", TLArg("False", "Bypass"));

            newEnabledExtensionNames.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
            chainInstanceCreateInfo.enabledExtensionNames = newEnabledExtensionNames.data();
            chainInstanceCreateInfo.enabledExtensionCount = (uint32_t)newEnabledExtensionNames.size();
        } else {
            TraceLoggingWrite(g_traceProvider, "xrCreateApiLayerInstance", TLArg("True", "Bypass"));

            if (!(want_XR_KHR_vulkan_enable || want_XR_KHR_vulkan_enable2 || want_XR_KHR_opengl_enable)) {
                Log("Vulkan/OpenGL is not requested for the instance\n");
            } else {
                Log("Vulkan/OpenGL is already implemented upstream of the layer\n");
            }

            // Call the chain to create the instance, and nothing else.
            XrApiLayerCreateInfo chainApiLayerInfo = *apiLayerInfo;
            chainApiLayerInfo.nextInfo = apiLayerInfo->nextInfo->next;
            const XrResult result =
                apiLayerInfo->nextInfo->nextCreateApiLayerInstance(instanceCreateInfo, &chainApiLayerInfo, instance);

            if (XR_SUCCEEDED(result)) {
                std::unique_lock lock(g_bypassLock);

                // Bypass interception of xrGetInstanceProcAddr() calls.
                g_bypass.insert_or_assign(*instance, apiLayerInfo->nextInfo->nextGetInstanceProcAddr);
            }

            TraceLoggingWrite(
                g_traceProvider, "xrCreateApiLayerInstance_Result", TLArg(xr::ToCString(result), "Result"));

            DebugLog("<-- xrCreateApiLayerInstance %d\n", result);

            return result;
        }

        // Call the chain to create the instance.
        XrApiLayerCreateInfo chainApiLayerInfo = *apiLayerInfo;
        chainApiLayerInfo.nextInfo = apiLayerInfo->nextInfo->next;
        XrResult result =
            apiLayerInfo->nextInfo->nextCreateApiLayerInstance(&chainInstanceCreateInfo, &chainApiLayerInfo, instance);
        if (result == XR_SUCCESS) {
            // Make sure any prior bypass is cleared (in case the XrInstance handle is re-used).
            {
                std::unique_lock lock(g_bypassLock);

                g_bypass.erase(*instance);
            }

            // Create our layer.
            LAYER_NAMESPACE::GetInstance()->SetGetInstanceProcAddr(apiLayerInfo->nextInfo->nextGetInstanceProcAddr,
                                                                   *instance);

            // Forward the xrCreateInstance() call to the layer.
            try {
                result = LAYER_NAMESPACE::GetInstance()->xrCreateInstance(instanceCreateInfo);
            } catch (std::runtime_error exc) {
                TraceLoggingWrite(g_traceProvider, "xrCreateInstance_Error", TLArg(exc.what(), "Error"));
                ErrorLog("xrCreateInstance: %s\\n", exc.what());
                result = XR_ERROR_RUNTIME_FAILURE;
            }

            // Cleanup before returning an error.
            if (XR_FAILED(result)) {
                PFN_xrDestroyInstance xrDestroyInstance;
                CHECK_XRCMD(apiLayerInfo->nextInfo->nextGetInstanceProcAddr(
                    *instance, "xrDestroyInstance", reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyInstance)));
                xrDestroyInstance(*instance);
            }
        }

        TraceLoggingWrite(g_traceProvider, "xrCreateApiLayerInstance_Result", TLArg(xr::ToCString(result), "Result"));
        if (XR_FAILED(result)) {
            ErrorLog("xrCreateApiLayerInstance failed with %s\\n", xr::ToCString(result));
        }

        DebugLog("<-- xrCreateApiLayerInstance %d\n", result);

        return result;
    }

    // Handle cleanup of the layer's singleton.
    XrResult XRAPI_CALL xrDestroyInstance(XrInstance instance) {
        TraceLoggingWrite(g_traceProvider, "xrDestroyInstance");

        XrResult result;
        try {
            result = LAYER_NAMESPACE::GetInstance()->xrDestroyInstance(instance);
            if (XR_SUCCEEDED(result)) {
                LAYER_NAMESPACE::ResetInstance();
            }
        } catch (std::runtime_error exc) {
            TraceLoggingWrite(g_traceProvider, "xrDestroyInstance_Error", TLArg(exc.what(), "Error"));
            ErrorLog("xrDestroyInstance: %s\\n", exc.what());
            result = XR_ERROR_RUNTIME_FAILURE;
        }

        TraceLoggingWrite(g_traceProvider, "xrDestroyInstance_Result", TLArg(xr::ToCString(result), "Result"));
        if (XR_FAILED(result)) {
            ErrorLog("xrDestroyInstance failed with %s\\n", xr::ToCString(result));
        }

        return result;
    }

    // Forward the xrGetInstanceProcAddr() call to the dispatcher.
    XrResult XRAPI_CALL xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function) {
        {
            std::unique_lock lock(g_bypassLock);

            // Bypass entirely the layer if requested.
            const auto cit = g_bypass.find(instance);
            if (cit != g_bypass.cend()) {
                return cit->second(instance, name, function);
            }
        }

        TraceLoggingWrite(g_traceProvider, "xrGetInstanceProcAddr");

        XrResult result;
        try {
            result = LAYER_NAMESPACE::GetInstance()->xrGetInstanceProcAddr(instance, name, function);
        } catch (std::runtime_error exc) {
            TraceLoggingWrite(g_traceProvider, "xrGetInstanceProcAddr_Error", TLArg(exc.what(), "Error"));
            ErrorLog("xrGetInstanceProcAddr: %s\\n", exc.what());
            result = XR_ERROR_RUNTIME_FAILURE;
        }

        TraceLoggingWrite(g_traceProvider, "xrGetInstanceProcAddr_Result", TLArg(xr::ToCString(result), "Result"));

        return result;
    }

} // namespace LAYER_NAMESPACE
