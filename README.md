# OpenXR Vulkan on Direct3D 12 interoperability layer

This software enables OpenXR apps developed for Vulkan to work with OpenXR runtimes that have support for Direct3D 12 and not Vulkan.

DISCLAIMER: This software is distributed as-is, without any warranties or conditions of any kind. Use at your own risks.

## Setup

Download the latest version from the [Releases page](https://github.com/mbucchia/OpenXR-Vk-D3D12/releases). Find the installer program under **Assets**, file `OpenXR-Vk-D3D12.msi`.

For troubleshooting, the log file can be found at `%LocalAppData%\XR_APILAYER_NOVENDOR_vulkan_d3d12_interop.log`.

## Limitations

- This has only been tested with Windows Mixed Reality.
- This has only been tested with NVIDIA.
- This has been tested with the HelloXR sample app from Khronos, and with Unity 2021.

## Known issues

- Applications using legacy `XR_KHR_vulkan_enable` must enable Vulkan timeline semaphores at device creation time.

```
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES};
    timelineSemaphoreFeatures.timelineSemaphore = true;

    // Chain timelineSemaphoreFeatures to your VkDeviceCreateInfo struct.
```
