%VULKAN_SDK%/Bin/glslc.exe FXAA.comp -DOSK_VULKAN -o FXAA.spv
%VULKAN_SDK%/Bin/glslc.exe ToneMapping.comp -DOSK_VULKAN -o ToneMapping.spv

%VULKAN_SDK%/Bin/glslc.exe HBAO/hbao.comp -DOSK_VULKAN -o HBAO/hbao.spv
%VULKAN_SDK%/Bin/glslc.exe HBAO/Blur.comp -DOSK_VULKAN -o HBAO/Blur.spv
%VULKAN_SDK%/Bin/glslc.exe HBAO/Resolve.comp -DOSK_VULKAN -o HBAO/Resolve.spv

%VULKAN_SDK%/Bin/glslc.exe SMAA/edge_detection.comp -DOSK_VULKAN -o SMAA/edge_detection.spv

PAUSE