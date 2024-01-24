%VULKAN_SDK%/Bin/glslc.exe bloom_downscale.comp -DOSK_VULKAN -o bloom_downscale.spv
%VULKAN_SDK%/Bin/glslc.exe bloom_upscale.comp -DOSK_VULKAN -o bloom_upscale.spv
%VULKAN_SDK%/Bin/glslc.exe bloom_final.comp -DOSK_VULKAN -o bloom_final.spv

PAUSE