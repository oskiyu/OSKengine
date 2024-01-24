%VULKAN_SDK%/Bin/glslc.exe shader.vert -DOSK_VULKAN -o vert.spv
%VULKAN_SDK%/Bin/glslc.exe shader.frag -DOSK_VULKAN -o frag.spv

%VULKAN_SDK%/Bin/glslc.exe Animated/shader.vert -DOSK_VULKAN -o Animated/vert.spv

PAUSE