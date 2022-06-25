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

#include "pch.h"

#include <layer.h>

#include "dispatch.h"
#include "log.h"

#ifndef LAYER_NAMESPACE
#error Must define LAYER_NAMESPACE
#endif

using namespace LAYER_NAMESPACE::log;

namespace LAYER_NAMESPACE
{

	// Auto-generated wrappers for the requested APIs.

	XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId)
	{
		DebugLog("--> xrGetSystem\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetSystem(instance, getInfo, systemId);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetSystem %d\n", result);

		return result;
	}

	XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session)
	{
		DebugLog("--> xrCreateSession\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateSession(instance, createInfo, session);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrCreateSession %d\n", result);

		return result;
	}

	XrResult xrDestroySession(XrSession session)
	{
		DebugLog("--> xrDestroySession\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrDestroySession(session);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrDestroySession %d\n", result);

		return result;
	}

	XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats)
	{
		DebugLog("--> xrEnumerateSwapchainFormats\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEnumerateSwapchainFormats(session, formatCapacityInput, formatCountOutput, formats);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrEnumerateSwapchainFormats %d\n", result);

		return result;
	}

	XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain)
	{
		DebugLog("--> xrCreateSwapchain\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateSwapchain(session, createInfo, swapchain);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrCreateSwapchain %d\n", result);

		return result;
	}

	XrResult xrDestroySwapchain(XrSwapchain swapchain)
	{
		DebugLog("--> xrDestroySwapchain\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrDestroySwapchain(swapchain);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrDestroySwapchain %d\n", result);

		return result;
	}

	XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images)
	{
		DebugLog("--> xrEnumerateSwapchainImages\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEnumerateSwapchainImages(swapchain, imageCapacityInput, imageCountOutput, images);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrEnumerateSwapchainImages %d\n", result);

		return result;
	}

	XrResult xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo)
	{
		DebugLog("--> xrEndFrame\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEndFrame(session, frameEndInfo);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrEndFrame %d\n", result);

		return result;
	}

	XrResult xrGetVulkanInstanceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer)
	{
		DebugLog("--> xrGetVulkanInstanceExtensionsKHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanInstanceExtensionsKHR(instance, systemId, bufferCapacityInput, bufferCountOutput, buffer);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetVulkanInstanceExtensionsKHR %d\n", result);

		return result;
	}

	XrResult xrGetVulkanDeviceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer)
	{
		DebugLog("--> xrGetVulkanDeviceExtensionsKHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanDeviceExtensionsKHR(instance, systemId, bufferCapacityInput, bufferCountOutput, buffer);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetVulkanDeviceExtensionsKHR %d\n", result);

		return result;
	}

	XrResult xrGetVulkanGraphicsDeviceKHR(XrInstance instance, XrSystemId systemId, VkInstance vkInstance, VkPhysicalDevice* vkPhysicalDevice)
	{
		DebugLog("--> xrGetVulkanGraphicsDeviceKHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsDeviceKHR(instance, systemId, vkInstance, vkPhysicalDevice);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetVulkanGraphicsDeviceKHR %d\n", result);

		return result;
	}

	XrResult xrGetVulkanGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements)
	{
		DebugLog("--> xrGetVulkanGraphicsRequirementsKHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsRequirementsKHR(instance, systemId, graphicsRequirements);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetVulkanGraphicsRequirementsKHR %d\n", result);

		return result;
	}

	XrResult xrCreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo, VkInstance* vulkanInstance, VkResult* vulkanResult)
	{
		DebugLog("--> xrCreateVulkanInstanceKHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrCreateVulkanInstanceKHR %d\n", result);

		return result;
	}

	XrResult xrCreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo, VkDevice* vulkanDevice, VkResult* vulkanResult)
	{
		DebugLog("--> xrCreateVulkanDeviceKHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrCreateVulkanDeviceKHR %d\n", result);

		return result;
	}

	XrResult xrGetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo, VkPhysicalDevice* vulkanPhysicalDevice)
	{
		DebugLog("--> xrGetVulkanGraphicsDevice2KHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetVulkanGraphicsDevice2KHR %d\n", result);

		return result;
	}

	XrResult xrGetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements)
	{
		DebugLog("--> xrGetVulkanGraphicsRequirements2KHR\n");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
		}
		catch (std::exception exc)
		{
			Log("%s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		DebugLog("<-- xrGetVulkanGraphicsRequirements2KHR %d\n", result);

		return result;
	}


	// Auto-generated dispatcher handler.
	XrResult OpenXrApi::xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function)
	{
		XrResult result = m_xrGetInstanceProcAddr(instance, name, function);

		const std::string apiName(name);

		if (apiName == "xrDestroyInstance")
		{
			m_xrDestroyInstance = reinterpret_cast<PFN_xrDestroyInstance>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrDestroyInstance);
		}
		else if (apiName == "xrGetSystem")
		{
			m_xrGetSystem = reinterpret_cast<PFN_xrGetSystem>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetSystem);
		}
		else if (apiName == "xrCreateSession")
		{
			m_xrCreateSession = reinterpret_cast<PFN_xrCreateSession>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrCreateSession);
		}
		else if (apiName == "xrDestroySession")
		{
			m_xrDestroySession = reinterpret_cast<PFN_xrDestroySession>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrDestroySession);
		}
		else if (apiName == "xrEnumerateSwapchainFormats")
		{
			m_xrEnumerateSwapchainFormats = reinterpret_cast<PFN_xrEnumerateSwapchainFormats>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrEnumerateSwapchainFormats);
		}
		else if (apiName == "xrCreateSwapchain")
		{
			m_xrCreateSwapchain = reinterpret_cast<PFN_xrCreateSwapchain>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrCreateSwapchain);
		}
		else if (apiName == "xrDestroySwapchain")
		{
			m_xrDestroySwapchain = reinterpret_cast<PFN_xrDestroySwapchain>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrDestroySwapchain);
		}
		else if (apiName == "xrEnumerateSwapchainImages")
		{
			m_xrEnumerateSwapchainImages = reinterpret_cast<PFN_xrEnumerateSwapchainImages>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrEnumerateSwapchainImages);
		}
		else if (apiName == "xrEndFrame")
		{
			m_xrEndFrame = reinterpret_cast<PFN_xrEndFrame>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrEndFrame);
		}
		else if (apiName == "xrGetVulkanInstanceExtensionsKHR")
		{
			m_xrGetVulkanInstanceExtensionsKHR = reinterpret_cast<PFN_xrGetVulkanInstanceExtensionsKHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanInstanceExtensionsKHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrGetVulkanDeviceExtensionsKHR")
		{
			m_xrGetVulkanDeviceExtensionsKHR = reinterpret_cast<PFN_xrGetVulkanDeviceExtensionsKHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanDeviceExtensionsKHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrGetVulkanGraphicsDeviceKHR")
		{
			m_xrGetVulkanGraphicsDeviceKHR = reinterpret_cast<PFN_xrGetVulkanGraphicsDeviceKHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsDeviceKHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrGetVulkanGraphicsRequirementsKHR")
		{
			m_xrGetVulkanGraphicsRequirementsKHR = reinterpret_cast<PFN_xrGetVulkanGraphicsRequirementsKHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsRequirementsKHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrCreateVulkanInstanceKHR")
		{
			m_xrCreateVulkanInstanceKHR = reinterpret_cast<PFN_xrCreateVulkanInstanceKHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrCreateVulkanInstanceKHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrCreateVulkanDeviceKHR")
		{
			m_xrCreateVulkanDeviceKHR = reinterpret_cast<PFN_xrCreateVulkanDeviceKHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrCreateVulkanDeviceKHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrGetVulkanGraphicsDevice2KHR")
		{
			m_xrGetVulkanGraphicsDevice2KHR = reinterpret_cast<PFN_xrGetVulkanGraphicsDevice2KHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsDevice2KHR);
			result = XR_SUCCESS;
		}
		else if (apiName == "xrGetVulkanGraphicsRequirements2KHR")
		{
			m_xrGetVulkanGraphicsRequirements2KHR = reinterpret_cast<PFN_xrGetVulkanGraphicsRequirements2KHR>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsRequirements2KHR);
			result = XR_SUCCESS;
		}


		return result;
	}

	// Auto-generated create instance handler.
	XrResult OpenXrApi::xrCreateInstance(const XrInstanceCreateInfo* createInfo)
    {
		if (XR_FAILED(m_xrGetInstanceProcAddr(m_instance, "xrGetInstanceProperties", reinterpret_cast<PFN_xrVoidFunction*>(&m_xrGetInstanceProperties))))
		{
			throw new std::runtime_error("Failed to resolve xrGetInstanceProperties");
		}
		if (XR_FAILED(m_xrGetInstanceProcAddr(m_instance, "xrGetSystemProperties", reinterpret_cast<PFN_xrVoidFunction*>(&m_xrGetSystemProperties))))
		{
			throw new std::runtime_error("Failed to resolve xrGetSystemProperties");
		}
		m_applicationName = createInfo->applicationInfo.applicationName;
		return XR_SUCCESS;
	}

} // namespace LAYER_NAMESPACE

