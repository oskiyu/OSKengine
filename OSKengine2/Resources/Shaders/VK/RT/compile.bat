C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe raygen.rgen --target-env=vulkan1.3 -o raygen.spv
C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe closesthit.rchit --target-env=vulkan1.3 -o closesthit.spv
C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe miss.rmiss --target-env=vulkan1.3 -o miss.spv

C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe Shadows/raygen.rgen --target-env=vulkan1.3 -o Shadows/raygen.spv
C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe Shadows/closesthit.rchit --target-env=vulkan1.3 -o Shadows/closesthit.spv
C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe Shadows/miss.rmiss --target-env=vulkan1.3 -o Shadows/miss.spv

C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe Shadows/Denoiser/denoiser.comp --target-env=vulkan1.3 -o Shadows/Denoiser/denoiser.spv
C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe Shadows/Denoiser/reprojection.comp --target-env=vulkan1.3 -o Shadows/Denoiser/reprojection.spv

C:/Users/Oscar/source/repos/OSKengine/Dependencies/OSKengine/VulkanSDK/Bin/glslc.exe Resolve/resolve.comp --target-env=vulkan1.3 -o Resolve/resolve.spv

PAUSE
