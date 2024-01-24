%VULKAN_SDK%/Bin/glslc.exe Shadows/raygen.rgen --target-env=vulkan1.3 -o Shadows/raygen.spv
%VULKAN_SDK%/Bin/glslc.exe Shadows/closesthit.rchit --target-env=vulkan1.3 -o Shadows/closesthit.spv
%VULKAN_SDK%/Bin/glslc.exe Shadows/miss.rmiss --target-env=vulkan1.3 -o Shadows/miss.spv

%VULKAN_SDK%/Bin/glslc.exe Shadows/Denoiser/denoiser.comp --target-env=vulkan1.3 -o Shadows/Denoiser/denoiser.spv
%VULKAN_SDK%/Bin/glslc.exe Shadows/Denoiser/reprojection.comp --target-env=vulkan1.3 -o Shadows/Denoiser/reprojection.spv

%VULKAN_SDK%/Bin/glslc.exe Resolve/resolve.comp --target-env=vulkan1.3 -o Resolve/resolve.spv

PAUSE
