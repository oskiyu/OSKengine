#include "MaterialLayoutBuilder.h"

#include "SpirvReflectionData.h"


static std::string GetSlotName(const std::unordered_map<OSK::UIndex32, std::string>& names, OSK::UIndex32 index) {
	auto nameIt = names.find(index);

	return nameIt != names.end()
		? nameIt.operator*().second
		: std::to_string(index);
}


OSK::GRAPHICS::MaterialLayoutBuilder::MaterialLayoutBuilder(const std::unordered_map<UIndex32, std::string>& slotNames, const std::string& materialName)
	: m_target(new MaterialLayout(materialName)), m_slotNames(slotNames) {

}

void OSK::GRAPHICS::MaterialLayoutBuilder::Apply(const SpirvReflectionData& shaderData) {	
	for (auto [slotIndex, shaderSlot] : shaderData.GetSlots()) {
		shaderSlot.name = GetSlotName(m_slotNames, shaderSlot.glslSetIndex);

		if (!m_target->GetAllSlots().contains(shaderSlot.name)) {
			m_target->AddSlot(shaderSlot);
		}
		else {
			EFTraits::AddFlag(&m_target->GetSlot(shaderSlot.name).stage, shaderData.GetShaderStage());
		}

		auto& targetSlot = m_target->GetSlot(shaderSlot.name);

		for (auto& [bindingName, shaderBinding] : shaderSlot.bindings) {

			if (targetSlot.bindings.contains(shaderBinding.name)) {
				continue;
			}

			auto newBinding = shaderBinding;

			shaderBinding.hlslDescriptorIndex = numHlslBindings;
			numHlslBindings++;

			if (shaderBinding.type == ShaderBindingType::UNIFORM_BUFFER || shaderBinding.type == ShaderBindingType::STORAGE_BUFFER) {
				shaderBinding.hlslIndex = numHlslBuffers;
				numHlslBuffers++;
			}
			else if (shaderBinding.type != ShaderBindingType::RT_ACCELERATION_STRUCTURE) {
				shaderBinding.hlslIndex = numHlslImages;
				numHlslImages++;
			}

			targetSlot.bindings[newBinding.name] = newBinding;
		}
	}

	// PushConstants
	for (const auto& pushConst : shaderData.GetPushConstants()) {

		// Si el push constant ya fue definido, actualizar su stage.
		bool found = false;
		for (auto& pConst : m_target->GetAllPushConstants()) {
			if (pConst.second.name == pushConst.name) {
				EFTraits::AddFlag(&pConst.second.stage, pushConst.stage);

				found = true;
			}
		}

		if (found)
			continue;

		MaterialLayoutPushConstant pushConstantInfo = pushConst;
		pushConstantInfo.offset = pushConstantsOffset;
		pushConstantInfo.hlslIndex = numHlslBuffers;
		pushConstantInfo.hlslBindingIndex = numHlslBindings;

		numHlslBuffers++;
		numHlslBindings++;

		pushConstantsOffset += pushConstantInfo.offset;

		m_target->AddPushConstant(pushConstantInfo);
	}
}

OSK::OwnedPtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::MaterialLayoutBuilder::Build() {
	return m_target.Release();
}
