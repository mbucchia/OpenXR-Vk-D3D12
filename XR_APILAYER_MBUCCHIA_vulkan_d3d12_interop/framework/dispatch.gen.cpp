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

	XrResult XRAPI_CALL xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetSystem");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetSystem(instance, getInfo, systemId);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetSystem_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetSystem: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetSystem_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetSystem failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session)
	{
		TraceLoggingWrite(g_traceProvider, "xrCreateSession");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateSession(instance, createInfo, session);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrCreateSession_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrCreateSession: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrCreateSession_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrCreateSession failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrDestroySession(XrSession session)
	{
		TraceLoggingWrite(g_traceProvider, "xrDestroySession");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrDestroySession(session);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrDestroySession_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrDestroySession: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrDestroySession_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrDestroySession failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views)
	{
		TraceLoggingWrite(g_traceProvider, "xrEnumerateViewConfigurationViews");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEnumerateViewConfigurationViews(instance, systemId, viewConfigurationType, viewCapacityInput, viewCountOutput, views);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrEnumerateViewConfigurationViews_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrEnumerateViewConfigurationViews: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrEnumerateViewConfigurationViews_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrEnumerateViewConfigurationViews failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats)
	{
		TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainFormats");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEnumerateSwapchainFormats(session, formatCapacityInput, formatCountOutput, formats);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainFormats_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrEnumerateSwapchainFormats: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainFormats_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrEnumerateSwapchainFormats failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain)
	{
		TraceLoggingWrite(g_traceProvider, "xrCreateSwapchain");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateSwapchain(session, createInfo, swapchain);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrCreateSwapchain_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrCreateSwapchain: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrCreateSwapchain_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrCreateSwapchain failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrDestroySwapchain(XrSwapchain swapchain)
	{
		TraceLoggingWrite(g_traceProvider, "xrDestroySwapchain");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrDestroySwapchain(swapchain);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrDestroySwapchain_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrDestroySwapchain: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrDestroySwapchain_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrDestroySwapchain failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images)
	{
		TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainImages");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEnumerateSwapchainImages(swapchain, imageCapacityInput, imageCountOutput, images);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainImages_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrEnumerateSwapchainImages: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrEnumerateSwapchainImages_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrEnumerateSwapchainImages failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index)
	{
		TraceLoggingWrite(g_traceProvider, "xrAcquireSwapchainImage");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrAcquireSwapchainImage(swapchain, acquireInfo, index);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrAcquireSwapchainImage_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrAcquireSwapchainImage: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrAcquireSwapchainImage_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrAcquireSwapchainImage failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo)
	{
		TraceLoggingWrite(g_traceProvider, "xrReleaseSwapchainImage");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrReleaseSwapchainImage(swapchain, releaseInfo);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrReleaseSwapchainImage_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrReleaseSwapchainImage: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrReleaseSwapchainImage_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrReleaseSwapchainImage failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo)
	{
		TraceLoggingWrite(g_traceProvider, "xrEndFrame");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrEndFrame(session, frameEndInfo);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrEndFrame_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrEndFrame: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrEndFrame_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrEndFrame failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetOpenGLGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLKHR* graphicsRequirements)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetOpenGLGraphicsRequirementsKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetOpenGLGraphicsRequirementsKHR(instance, systemId, graphicsRequirements);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetOpenGLGraphicsRequirementsKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetOpenGLGraphicsRequirementsKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetOpenGLGraphicsRequirementsKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetOpenGLGraphicsRequirementsKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetVulkanInstanceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetVulkanInstanceExtensionsKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanInstanceExtensionsKHR(instance, systemId, bufferCapacityInput, bufferCountOutput, buffer);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetVulkanInstanceExtensionsKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetVulkanInstanceExtensionsKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetVulkanInstanceExtensionsKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetVulkanInstanceExtensionsKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetVulkanDeviceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetVulkanDeviceExtensionsKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanDeviceExtensionsKHR(instance, systemId, bufferCapacityInput, bufferCountOutput, buffer);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetVulkanDeviceExtensionsKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetVulkanDeviceExtensionsKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetVulkanDeviceExtensionsKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetVulkanDeviceExtensionsKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetVulkanGraphicsDeviceKHR(XrInstance instance, XrSystemId systemId, VkInstance vkInstance, VkPhysicalDevice* vkPhysicalDevice)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsDeviceKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsDeviceKHR(instance, systemId, vkInstance, vkPhysicalDevice);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsDeviceKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetVulkanGraphicsDeviceKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsDeviceKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetVulkanGraphicsDeviceKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetVulkanGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsRequirementsKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsRequirementsKHR(instance, systemId, graphicsRequirements);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsRequirementsKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetVulkanGraphicsRequirementsKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsRequirementsKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetVulkanGraphicsRequirementsKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrCreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo, VkInstance* vulkanInstance, VkResult* vulkanResult)
	{
		TraceLoggingWrite(g_traceProvider, "xrCreateVulkanInstanceKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrCreateVulkanInstanceKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrCreateVulkanInstanceKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrCreateVulkanInstanceKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrCreateVulkanInstanceKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrCreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo, VkDevice* vulkanDevice, VkResult* vulkanResult)
	{
		TraceLoggingWrite(g_traceProvider, "xrCreateVulkanDeviceKHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrCreateVulkanDeviceKHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrCreateVulkanDeviceKHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrCreateVulkanDeviceKHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrCreateVulkanDeviceKHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo, VkPhysicalDevice* vulkanPhysicalDevice)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsDevice2KHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsDevice2KHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetVulkanGraphicsDevice2KHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsDevice2KHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetVulkanGraphicsDevice2KHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}

	XrResult XRAPI_CALL xrGetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements)
	{
		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsRequirements2KHR");

		XrResult result;
		try
		{
			result = LAYER_NAMESPACE::GetInstance()->xrGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
		}
		catch (std::exception exc)
		{
			TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsRequirements2KHR_Error", TLArg(exc.what(), "Error"));
			ErrorLog("xrGetVulkanGraphicsRequirements2KHR: %s\n", exc.what());
			result = XR_ERROR_RUNTIME_FAILURE;
		}

		TraceLoggingWrite(g_traceProvider, "xrGetVulkanGraphicsRequirements2KHR_Result", TLArg(xr::ToCString(result), "Result"));
		if (XR_FAILED(result)) {
			ErrorLog("xrGetVulkanGraphicsRequirements2KHR failed with %s\n", xr::ToCString(result));
		}

		return result;
	}


	// Auto-generated dispatcher handler.
	XrResult OpenXrApi::xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function)
	{
		*function = nullptr;
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
		else if (apiName == "xrEnumerateViewConfigurationViews")
		{
			m_xrEnumerateViewConfigurationViews = reinterpret_cast<PFN_xrEnumerateViewConfigurationViews>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrEnumerateViewConfigurationViews);
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
		else if (apiName == "xrAcquireSwapchainImage")
		{
			m_xrAcquireSwapchainImage = reinterpret_cast<PFN_xrAcquireSwapchainImage>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrAcquireSwapchainImage);
		}
		else if (apiName == "xrReleaseSwapchainImage")
		{
			m_xrReleaseSwapchainImage = reinterpret_cast<PFN_xrReleaseSwapchainImage>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrReleaseSwapchainImage);
		}
		else if (apiName == "xrEndFrame")
		{
			m_xrEndFrame = reinterpret_cast<PFN_xrEndFrame>(*function);
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrEndFrame);
		}
		else if (has_XR_KHR_opengl_enable && apiName == "xrGetOpenGLGraphicsRequirementsKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetOpenGLGraphicsRequirementsKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable && apiName == "xrGetVulkanInstanceExtensionsKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanInstanceExtensionsKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable && apiName == "xrGetVulkanDeviceExtensionsKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanDeviceExtensionsKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable && apiName == "xrGetVulkanGraphicsDeviceKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsDeviceKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable && apiName == "xrGetVulkanGraphicsRequirementsKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsRequirementsKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable2 && apiName == "xrCreateVulkanInstanceKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrCreateVulkanInstanceKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable2 && apiName == "xrCreateVulkanDeviceKHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrCreateVulkanDeviceKHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable2 && apiName == "xrGetVulkanGraphicsDevice2KHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsDevice2KHR);
			result = XR_SUCCESS;
		}
		else if (has_XR_KHR_vulkan_enable2 && apiName == "xrGetVulkanGraphicsRequirements2KHR") {
			*function = reinterpret_cast<PFN_xrVoidFunction>(LAYER_NAMESPACE::xrGetVulkanGraphicsRequirements2KHR);
			result = XR_SUCCESS;
		}


		return result;
	}

	// Auto-generated create instance handler.
	XrResult OpenXrApi::xrCreateInstance(const XrInstanceCreateInfo* createInfo)
	{
		for (uint32_t i = 0; i < createInfo->enabledExtensionCount; i++) {
			const std::string_view ext(createInfo->enabledExtensionNames[i]);
			if (false) {
			}
			else if (ext == "XR_KHR_vulkan_enable") {
				has_XR_KHR_vulkan_enable = true;
			}
			else if (ext == "XR_KHR_vulkan_enable2") {
				has_XR_KHR_vulkan_enable2 = true;
			}
			else if (ext == "XR_KHR_opengl_enable") {
				has_XR_KHR_opengl_enable = true;
			}
			else if (ext == "XR_KHR_D3D12_enable") {
				has_XR_KHR_D3D12_enable = true;
			}
			else if (ext == "XR_KHR_composition_layer_depth") {
				has_XR_KHR_composition_layer_depth = true;
			}
			else if (ext == "XR_KHR_composition_layer_cylinder") {
				has_XR_KHR_composition_layer_cylinder = true;
			}
			else if (ext == "XR_KHR_composition_layer_equirect") {
				has_XR_KHR_composition_layer_equirect = true;
			}
			else if (ext == "XR_KHR_composition_layer_equirect2") {
				has_XR_KHR_composition_layer_equirect2 = true;
			}

		}
		if (XR_FAILED(m_xrGetInstanceProcAddr(m_instance, "xrGetInstanceProperties", reinterpret_cast<PFN_xrVoidFunction*>(&m_xrGetInstanceProperties))))
		{
			throw new std::runtime_error("Failed to resolve xrGetInstanceProperties");
		}
		if (XR_FAILED(m_xrGetInstanceProcAddr(m_instance, "xrGetSystemProperties", reinterpret_cast<PFN_xrVoidFunction*>(&m_xrGetSystemProperties))))
		{
			throw new std::runtime_error("Failed to resolve xrGetSystemProperties");
		}
		if (XR_FAILED(m_xrGetInstanceProcAddr(m_instance, "xrGetViewConfigurationProperties", reinterpret_cast<PFN_xrVoidFunction*>(&m_xrGetViewConfigurationProperties))))
		{
			throw new std::runtime_error("Failed to resolve xrGetViewConfigurationProperties");
		}
		m_applicationName = createInfo->applicationInfo.applicationName;
		return XR_SUCCESS;
	}

} // namespace LAYER_NAMESPACE

