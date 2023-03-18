%VULKAN_SDK%/Bin/glslc.exe ./Direct/shader.vert -o ./Direct/vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Direct/shader.frag -o ./Direct/frag.spv

%VULKAN_SDK%/Bin/glslc.exe ./Direct/Animated/shader.vert -o ./Direct/Animated/vert.spv

%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/shader.vert -o ./Deferred/GBuffer/vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/shader.frag -o ./Deferred/GBuffer/frag.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/GBuffer/anim.vert -o ./Deferred/GBuffer/anim.spv

%VULKAN_SDK%/Bin/glslc.exe ./Deferred/Resolve/shader.vert -o ./Deferred/Resolve/vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Deferred/Resolve/shader.frag -o ./Deferred/Resolve/frag.spv

%VULKAN_SDK%/Bin/glslc.exe ./Terrain/shader.tesc -o ./Terrain/tesc.spv
%VULKAN_SDK%/Bin/glslc.exe ./Terrain/shader.tese -o ./Terrain/tese.spv
%VULKAN_SDK%/Bin/glslc.exe ./Terrain/shader.vert -o ./Terrain/vert.spv
%VULKAN_SDK%/Bin/glslc.exe ./Terrain/shader.frag -o ./Terrain/frag.spv

PAUSE
