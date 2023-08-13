%VULKAN_SDK%/Bin/glslc.exe FXAA.comp -DOSK_VULKAN -o FXAA.spv
%VULKAN_SDK%/Bin/glslc.exe ToneMapping.comp -DOSK_VULKAN -o ToneMapping.spv

%VULKAN_SDK%/Bin/glslc.exe SMAA/edge_detection.comp -DOSK_VULKAN -o SMAA/edge_detection.spv

PAUSE