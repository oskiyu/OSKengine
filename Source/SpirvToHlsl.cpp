#include "SpirvToHlsl.h"

#include "VertexInfo.h"
#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

SpirvToHlsl::SpirvToHlsl(const DynamicArray<char>& spirvBytecode) :
	compilerToHlsl(spirv_cross::CompilerHLSL((const uint32_t*)spirvBytecode.GetData(), spirvBytecode.GetSize() / 4)) {

	SetHlslTargetProfile(6, 1);
}

void SpirvToHlsl::SetVertexAttributesMapping(const VertexInfo& vertexInfo) {
	for (UIndex32 i = 0; i < vertexInfo.entries.GetSize(); i++)
		compilerToHlsl.add_vertex_attribute_remap({ i, vertexInfo.entries[i].GetName() });
}

void SpirvToHlsl::SetLayoutMapping(const MaterialLayout& layout) {
	for (const auto& [slotName, slot] : layout.GetAllSlots()) {

		for (const auto& binding : slot.bindings) {

			const static DynamicArray<ShaderStage> stages = { ShaderStage::VERTEX, ShaderStage::FRAGMENT, ShaderStage::TESSELATION_CONTROL, ShaderStage::TESSELATION_EVALUATION };

			for (const ShaderStage stage : stages) {
				spirv_cross::HLSLResourceBinding hlslBinding{};

				auto subbinding = spirv_cross::HLSLResourceBinding::Binding{};
				subbinding.register_binding = binding.second.hlslIndex;
				subbinding.register_space = 0;

				hlslBinding.desc_set = slot.glslSetIndex;
				hlslBinding.binding = binding.second.glslIndex;

				hlslBinding.sampler = subbinding;
				hlslBinding.cbv = subbinding;
				hlslBinding.srv = subbinding;
				hlslBinding.uav = subbinding;

				if (stage == ShaderStage::VERTEX)
					hlslBinding.stage = spv::ExecutionModel::ExecutionModelVertex;
				else if (stage == ShaderStage::FRAGMENT)
					hlslBinding.stage = spv::ExecutionModel::ExecutionModelFragment;
				else if (stage == ShaderStage::TESSELATION_CONTROL)
					hlslBinding.stage = spv::ExecutionModel::ExecutionModelTessellationControl;
				else if (stage == ShaderStage::TESSELATION_EVALUATION)
					hlslBinding.stage = spv::ExecutionModel::ExecutionModelTessellationEvaluation;
				
				compilerToHlsl.add_hlsl_resource_binding(hlslBinding);
			}
		}

		// Push constants
		std::vector<spirv_cross::RootConstants> pushConstants;

		for (const auto& i : layout.GetAllPushConstants()) {
			spirv_cross::RootConstants constants{};
			constants.space = 1;
			constants.binding = i.second.hlslIndex;
			constants.start = i.second.offset;
			constants.end = i.second.offset + i.second.size;

			pushConstants.push_back(constants);
		}

		compilerToHlsl.set_root_constant_layouts(pushConstants);
	}
}

void SpirvToHlsl::SetHlslTargetProfile(USize64 mayor, USize64 minor) {
	auto options = compilerToHlsl.get_hlsl_options();
	options.shader_model = mayor * 10 + minor;
	compilerToHlsl.set_hlsl_options(options);
}

std::string SpirvToHlsl::CreateHlsl() {
	return compilerToHlsl.compile();
}
