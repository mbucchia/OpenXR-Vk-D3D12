# The list of OpenXR functions our layer will override.
override_functions = [
    "xrGetSystem",
    "xrEnumerateSwapchainFormats",
    "xrEnumerateViewConfigurationViews",
    "xrCreateSession",
    "xrDestroySession",
    "xrCreateSwapchain",
    "xrDestroySwapchain",
    "xrEnumerateSwapchainImages",
    "xrEndFrame",
    "xrGetVulkanInstanceExtensionsKHR",
    "xrGetVulkanDeviceExtensionsKHR",
    "xrGetVulkanGraphicsDeviceKHR",
    "xrCreateVulkanInstanceKHR",
    "xrCreateVulkanDeviceKHR",
    "xrGetVulkanGraphicsDevice2KHR",
    "xrGetVulkanGraphicsRequirementsKHR",
    "xrGetVulkanGraphicsRequirements2KHR",
    "xrGetOpenGLGraphicsRequirementsKHR",
]

# The list of OpenXR functions our layer will use from the runtime.
# Might repeat entries from override_functions above.
requested_functions = [
    "xrGetInstanceProperties",
    "xrGetSystemProperties",
    "xrGetViewConfigurationProperties"
]

# The list of OpenXR extensions our layer may expose.
supported_extensions = ['XR_KHR_vulkan_enable', 'XR_KHR_vulkan_enable2', 'XR_KHR_opengl_enable', 'XR_KHR_D3D12_enable']
