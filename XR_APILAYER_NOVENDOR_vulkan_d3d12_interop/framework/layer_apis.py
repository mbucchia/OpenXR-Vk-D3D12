# The list of OpenXR functions our layer will override.
override_functions = [
    "xrGetSystem",
    "xrEnumerateSwapchainFormats",
    "xrCreateSession",
    "xrDestroySession",
    "xrBeginSession",
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
