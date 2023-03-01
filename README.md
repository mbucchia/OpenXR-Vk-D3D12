# OpenXR Vulkan/OpenGL on Direct3D 12 interoperability layer

This software enables OpenXR apps developed for Vulkan/OpenGL to work with OpenXR runtimes that have support for Direct3D 12 and not Vulkan/OpenGL.

DISCLAIMER: This software is distributed as-is, without any warranties or conditions of any kind. Use at your own risks.

## Setup

Download the latest version from the [Releases page](https://github.com/mbucchia/OpenXR-Vk-D3D12/releases). Find the installer program under **Assets**, file `OpenXR-Vk-D3D12.msi`.

For troubleshooting, the log file can be found at `%LocalAppData%\XR_APILAYER_MBUCCHIA_vulkan_d3d12_interop.log`.

## Limitations

- This has only been tested with Windows Mixed Reality.
- This has only been tested with NVIDIA (driver version 528.49) and AMD video cards (driver version 23.2.1).
- Vulkan support has been tested with the HelloXR sample app from Khronos, Unity 2021 and Godot 4.
- OpenGL support has been tested with the HelloXR sample app from Khronos and X-Plane 12 Demo (using [OpenComposite](https://gitlab.com/znixian/OpenOVR/-/tree/openxr)).
- It is compatible with [OpenXR Toolkit](https://mbucchia.github.io/OpenXR-Toolkit/).

## Known issues

- Applications using legacy `XR_KHR_vulkan_enable` must enable Vulkan timeline semaphores at device creation time.

```
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES};
    timelineSemaphoreFeatures.timelineSemaphore = true;

    // Chain timelineSemaphoreFeatures to your VkDeviceCreateInfo struct.
```

- Applications using OpenGL with an OpenXR runtimes without support for mutable FOV (as reported in `XrViewConfigurationProperties`) and applications using OpenGL and rendering quad layers will have rendering upside-down. The necessary code to Y-flip the image is not implemented.

If you are having issues, please visit the [Issues page](https://github.com/mbucchia/OpenXR-Vk-D3D12/issues) to look at existing support requests or to file a new one.

## OpenXR Conformance

The API layer passed all [OpenXR conformance tests](https://github.com/KhronosGroup/OpenXR-CTS) (at v1.0.26.0) with Vulkan as the graphics API and with Windows Mixed Reality at the backing OpenXR runtime.

## How does it work?

This API layer sits between any OpenXR application and the OpenXR runtime. It enhances the currently selected OpenXR runtime with the OpenXR extensions necessary for Vulkan support (`XR_KHR_vulkan_enable` and `XR_KHR_vulkan_enable2`). It uses the OpenXR runtime's Direct3D 12 support to efficiently bridge the application's Vulkan rendering to Direct3D 12. This processes does not add any overhead: the swapchains (drawing surfaces) requested by the application in Vulkan formats are imported as-is from Direct3D 12, there is no additional copy nor composition phase. Upon submission of the rendered frame, a simple fence synchronization primitive is inserted in the GPU queue shared with the OpenXR runtime, which will not block the application's rendering loop.

Same principle applies to OpenGL support.
