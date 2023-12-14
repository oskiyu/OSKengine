%VULKAN_SDK%/Bin/glslc.exe ./Direct/shader.vert -o ./Direct/vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Direct/shader.frag -o ./Direct/frag.spv

%VULKAN_SDK%/Bin/glslc.exe ./Direct/Animated/shader.vert -o ./Direct/Animated/vert.spv


%VULKAN_SDK%/Bin/glslc.exe ./Deferred/Tree/normals.vert -o ./Deferred/Tree/normals_vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/Tree/normals.frag -o ./Deferred/Tree/normals_frag.spv

%VULKAN_SDK%/Bin/glslc.exe ./Deferred/Tree/shader.vert -o ./Deferred/Tree/gbuffer_vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/Tree/shader.frag -o ./Deferred/Tree/gbuffer_frag.spv


%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/shader.vert -o ./Deferred/GBuffer/vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/shader.frag -o ./Deferred/GBuffer/frag.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/shader_billboard.frag -o ./Deferred/GBuffer/shader_billboard.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/anim.vert -o ./Deferred/GBuffer/anim.spv

%VULKAN_SDK%/Bin/glslc.exe ./TAA/taa.comp -o ./TAA/taa.spv
%VULKAN_SDK%/Bin/glslc.exe ./TAA/sharpen.comp -o ./TAA/sharpen.spv

%VULKAN_SDK%/Bin/glslc.exe ./Deferred/CompResolve/shader.comp -o ./Deferred/CompResolve/shader.spv

PAUSE
