%VULKAN_SDK%/Bin/glslc.exe ./mesh.task -o ./task.spv --target-env=vulkan1.3

%VULKAN_SDK%/Bin/glslc.exe ./mesh.mesh -o ./mesh_high.spv -DQUALITY_HIGH --target-env=vulkan1.3
%VULKAN_SDK%/Bin/glslc.exe ./mesh.mesh -o ./mesh_medium.spv -DQUALITY_MEDIUM --target-env=vulkan1.3
%VULKAN_SDK%/Bin/glslc.exe ./mesh.mesh -o ./mesh_low.spv -DQUALITY_LOW --target-env=vulkan1.3

%VULKAN_SDK%/Bin/glslc.exe ./shader.frag -o ./frag.spv
