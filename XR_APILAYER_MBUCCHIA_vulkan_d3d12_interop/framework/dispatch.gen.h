// *********** THIS FILE IS GENERATED - DO NOT EDIT ***********
// MIT License
//
// Copyright(c) 2021-2022 Matthieu Bucchianeri
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

#ifndef LAYER_NAMESPACE
#error Must define LAYER_NAMESPACE
#endif

namespace LAYER_NAMESPACE
{

	class OpenXrApi
	{
	private:
		XrInstance m_instance{ XR_NULL_HANDLE };
		std::string m_applicationName;

	protected:
		OpenXrApi() = default;

		PFN_xrGetInstanceProcAddr m_xrGetInstanceProcAddr{ nullptr };

	public:
		virtual ~OpenXrApi() = default;

		XrInstance GetXrInstance() const
		{
			return m_instance;
		}

		const std::string& GetApplicationName() const
		{
			return m_applicationName;
		}

		void SetGetInstanceProcAddr(PFN_xrGetInstanceProcAddr pfn_xrGetInstanceProcAddr, XrInstance instance)
		{
			m_xrGetInstanceProcAddr = pfn_xrGetInstanceProcAddr;
			m_instance = instance;
		}

		// Specially-handled by the auto-generated code.
		virtual XrResult xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function);
		virtual XrResult xrCreateInstance(const XrInstanceCreateInfo* createInfo);


		// Auto-generated entries for the requested APIs.

	public:
		virtual XrResult xrDestroyInstance(XrInstance instance)
		{
			return m_xrDestroyInstance(instance);
		}
	private:
		PFN_xrDestroyInstance m_xrDestroyInstance{ nullptr };

	public:
		virtual XrResult xrGetInstanceProperties(XrInstance instance, XrInstanceProperties* instanceProperties)
		{
			return m_xrGetInstanceProperties(instance, instanceProperties);
		}
	private:
		PFN_xrGetInstanceProperties m_xrGetInstanceProperties{ nullptr };

	public:
		virtual XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId)
		{
			return m_xrGetSystem(instance, getInfo, systemId);
		}
	private:
		PFN_xrGetSystem m_xrGetSystem{ nullptr };

	public:
		virtual XrResult xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties* properties)
		{
			return m_xrGetSystemProperties(instance, systemId, properties);
		}
	private:
		PFN_xrGetSystemProperties m_xrGetSystemProperties{ nullptr };

	public:
		virtual XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session)
		{
			return m_xrCreateSession(instance, createInfo, session);
		}
	private:
		PFN_xrCreateSession m_xrCreateSession{ nullptr };

	public:
		virtual XrResult xrDestroySession(XrSession session)
		{
			return m_xrDestroySession(session);
		}
	private:
		PFN_xrDestroySession m_xrDestroySession{ nullptr };

	public:
		virtual XrResult xrGetViewConfigurationProperties(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties* configurationProperties)
		{
			return m_xrGetViewConfigurationProperties(instance, systemId, viewConfigurationType, configurationProperties);
		}
	private:
		PFN_xrGetViewConfigurationProperties m_xrGetViewConfigurationProperties{ nullptr };

	public:
		virtual XrResult xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views)
		{
			return m_xrEnumerateViewConfigurationViews(instance, systemId, viewConfigurationType, viewCapacityInput, viewCountOutput, views);
		}
	private:
		PFN_xrEnumerateViewConfigurationViews m_xrEnumerateViewConfigurationViews{ nullptr };

	public:
		virtual XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats)
		{
			return m_xrEnumerateSwapchainFormats(session, formatCapacityInput, formatCountOutput, formats);
		}
	private:
		PFN_xrEnumerateSwapchainFormats m_xrEnumerateSwapchainFormats{ nullptr };

	public:
		virtual XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain)
		{
			return m_xrCreateSwapchain(session, createInfo, swapchain);
		}
	private:
		PFN_xrCreateSwapchain m_xrCreateSwapchain{ nullptr };

	public:
		virtual XrResult xrDestroySwapchain(XrSwapchain swapchain)
		{
			return m_xrDestroySwapchain(swapchain);
		}
	private:
		PFN_xrDestroySwapchain m_xrDestroySwapchain{ nullptr };

	public:
		virtual XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images)
		{
			return m_xrEnumerateSwapchainImages(swapchain, imageCapacityInput, imageCountOutput, images);
		}
	private:
		PFN_xrEnumerateSwapchainImages m_xrEnumerateSwapchainImages{ nullptr };

	public:
		virtual XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index)
		{
			return m_xrAcquireSwapchainImage(swapchain, acquireInfo, index);
		}
	private:
		PFN_xrAcquireSwapchainImage m_xrAcquireSwapchainImage{ nullptr };

	public:
		virtual XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo)
		{
			return m_xrReleaseSwapchainImage(swapchain, releaseInfo);
		}
	private:
		PFN_xrReleaseSwapchainImage m_xrReleaseSwapchainImage{ nullptr };

	public:
		virtual XrResult xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo)
		{
			return m_xrEndFrame(session, frameEndInfo);
		}
	private:
		PFN_xrEndFrame m_xrEndFrame{ nullptr };

	public:
		virtual XrResult xrGetOpenGLGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLKHR* graphicsRequirements)
		{
			return m_xrGetOpenGLGraphicsRequirementsKHR(instance, systemId, graphicsRequirements);
		}
	private:
		PFN_xrGetOpenGLGraphicsRequirementsKHR m_xrGetOpenGLGraphicsRequirementsKHR{ nullptr };

	public:
		virtual XrResult xrGetVulkanInstanceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer)
		{
			return m_xrGetVulkanInstanceExtensionsKHR(instance, systemId, bufferCapacityInput, bufferCountOutput, buffer);
		}
	private:
		PFN_xrGetVulkanInstanceExtensionsKHR m_xrGetVulkanInstanceExtensionsKHR{ nullptr };

	public:
		virtual XrResult xrGetVulkanDeviceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer)
		{
			return m_xrGetVulkanDeviceExtensionsKHR(instance, systemId, bufferCapacityInput, bufferCountOutput, buffer);
		}
	private:
		PFN_xrGetVulkanDeviceExtensionsKHR m_xrGetVulkanDeviceExtensionsKHR{ nullptr };

	public:
		virtual XrResult xrGetVulkanGraphicsDeviceKHR(XrInstance instance, XrSystemId systemId, VkInstance vkInstance, VkPhysicalDevice* vkPhysicalDevice)
		{
			return m_xrGetVulkanGraphicsDeviceKHR(instance, systemId, vkInstance, vkPhysicalDevice);
		}
	private:
		PFN_xrGetVulkanGraphicsDeviceKHR m_xrGetVulkanGraphicsDeviceKHR{ nullptr };

	public:
		virtual XrResult xrGetVulkanGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements)
		{
			return m_xrGetVulkanGraphicsRequirementsKHR(instance, systemId, graphicsRequirements);
		}
	private:
		PFN_xrGetVulkanGraphicsRequirementsKHR m_xrGetVulkanGraphicsRequirementsKHR{ nullptr };

	public:
		virtual XrResult xrCreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo, VkInstance* vulkanInstance, VkResult* vulkanResult)
		{
			return m_xrCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
		}
	private:
		PFN_xrCreateVulkanInstanceKHR m_xrCreateVulkanInstanceKHR{ nullptr };

	public:
		virtual XrResult xrCreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo, VkDevice* vulkanDevice, VkResult* vulkanResult)
		{
			return m_xrCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
		}
	private:
		PFN_xrCreateVulkanDeviceKHR m_xrCreateVulkanDeviceKHR{ nullptr };

	public:
		virtual XrResult xrGetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo, VkPhysicalDevice* vulkanPhysicalDevice)
		{
			return m_xrGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
		}
	private:
		PFN_xrGetVulkanGraphicsDevice2KHR m_xrGetVulkanGraphicsDevice2KHR{ nullptr };

	public:
		virtual XrResult xrGetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements)
		{
			return m_xrGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
		}
	private:
		PFN_xrGetVulkanGraphicsRequirements2KHR m_xrGetVulkanGraphicsRequirements2KHR{ nullptr };


	protected:
		// Auto-generated extension properties.
		bool has_XR_KHR_vulkan_enable{false};
		bool has_XR_KHR_vulkan_enable2{false};
		bool has_XR_KHR_opengl_enable{false};
		bool has_XR_KHR_D3D12_enable{false};
		bool has_XR_KHR_composition_layer_depth{false};
		bool has_XR_KHR_composition_layer_cylinder{false};
		bool has_XR_KHR_composition_layer_equirect{false};
		bool has_XR_KHR_composition_layer_equirect2{false};


	};

} // namespace LAYER_NAMESPACE

