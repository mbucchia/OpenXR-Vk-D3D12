$JsonPath = Join-Path "$PSScriptRoot" "XR_APILAYER_NOVENDOR_vulkan_d3d12_interop.json"
Start-Process -FilePath powershell.exe -Verb RunAs -Wait -ArgumentList @"
	& {
		Remove-ItemProperty -Path HKLM:\Software\Khronos\OpenXR\1\ApiLayers\Implicit -Name '$jsonPath' -Force | Out-Null
	}
"@
