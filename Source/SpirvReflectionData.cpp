#include "SpirvReflectionData.h"

#include <spirv_reflect.h>

#include "NumericTypes.h"
#include "MaterialLayoutBinding.h"

#include "DynamicArray.hpp"
#include "Assert.h"
#include "MaterialLayoutPushConstant.h"
#include "ShaderReflectionException.h"
#include "ShaderBindingType.h"

#include "Logger.h"
#include "OSKengine.h"


static OSK::GRAPHICS::ShaderBindingType ToBindingType(SpvReflectDescriptorType type) {
    using OSK::GRAPHICS::ShaderBindingType;

    switch (type) {
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return ShaderBindingType::TEXTURE;

    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return ShaderBindingType::STORAGE_IMAGE;

    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return ShaderBindingType::UNIFORM_BUFFER;

    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return ShaderBindingType::STORAGE_BUFFER;

    case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
        return ShaderBindingType::RT_ACCELERATION_STRUCTURE;

    default:
        OSK_ASSERT(false, OSK::ShaderReflectionException(std::format("Tipo de binding no soportado: {}.", std::to_underlying(type))));

        return{};

        break;
    }
}

static OSK::DynamicArray<SpvReflectBlockVariable*> GetPushConstantsReflection(const SpvReflectShaderModule& shaderModule) {
    uint32_t pushConstantsCount = 0;
    SpvReflectResult result = spvReflectEnumeratePushConstantBlocks(&shaderModule, &pushConstantsCount, nullptr);
    OSK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, OSK::ShaderReflectionException(std::format("No se pudo obtener informacion de los push constants: {}.", std::to_underlying(result))));

    OSK::DynamicArray<SpvReflectBlockVariable*> pushConstantsData = OSK::DynamicArray<SpvReflectBlockVariable*>::CreateResized(pushConstantsCount);
    result = spvReflectEnumeratePushConstantBlocks(&shaderModule, &pushConstantsCount, pushConstantsData.GetData());
    OSK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, OSK::ShaderReflectionException(std::format("No se pudo obtener informacion de los push constants: {}.", std::to_underlying(result))));

    return pushConstantsData;
}

static OSK::DynamicArray<SpvReflectDescriptorSet*> GetDescriptorsReflection(const SpvReflectShaderModule& shaderModule) {
    uint32_t slotCount = 0;
    SpvReflectResult result = spvReflectEnumerateDescriptorSets(&shaderModule, &slotCount, nullptr);
    OSK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, OSK::ShaderReflectionException(std::format("No se pudo obtener informacion de los descriptores: {}.", std::to_underlying(result))));

    OSK::DynamicArray<SpvReflectDescriptorSet*> shaderSlots = OSK::DynamicArray<SpvReflectDescriptorSet*>::CreateResized(slotCount);

    result = spvReflectEnumerateDescriptorSets(&shaderModule, &slotCount, shaderSlots.GetData());
    OSK_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, OSK::ShaderReflectionException(std::format("No se pudo obtener informacion de los descriptores: {}.", std::to_underlying(result))));

    return shaderSlots;
}


OSK::GRAPHICS::SpirvReflectionData::SpirvReflectionData(const DynamicArray<char>& bytecode, ShaderStage stage) 
: SpirvReflectionData(bytecode.GetFullSpan(), stage) {
    // Log(OSK::Engine::GetLogger());
}

OSK::GRAPHICS::SpirvReflectionData::SpirvReflectionData(std::span<const char> bytecode, ShaderStage stage) : m_stage(stage) {
	SpvReflectShaderModule shaderModule{};
	SpvReflectResult result = spvReflectCreateShaderModule(bytecode.size(), bytecode.data(), &shaderModule);

	OSK_ASSERT(
        result == SPV_REFLECT_RESULT_SUCCESS, 
        ShaderReflectionException(std::format("No se pudo obtener informacion del shader: {}.", std::to_underlying(result))));

    const auto shaderSlots = GetDescriptorsReflection(shaderModule);

    for (UIndex64 slotIndex = 0; slotIndex < shaderSlots.GetSize(); slotIndex++) {

        const SpvReflectDescriptorSet& slotData = *(shaderSlots[slotIndex]);

        MaterialLayoutSlot slot{};
        slot.glslSetIndex = slotData.set;
        slot.stage = stage;
        // slot.name establecido después.

        m_slots[slot.glslSetIndex] = slot;

        for (UIndex64 bindingIndex = 0; bindingIndex < slotData.binding_count; bindingIndex++) {

            const SpvReflectDescriptorBinding& bindingData = *(slotData.bindings[bindingIndex]);

            MaterialLayoutBinding binding{};
            binding.type = ToBindingType(bindingData.descriptor_type);
            binding.glslIndex = bindingData.binding;
            binding.name = std::string(bindingData.name);

            binding.numArrayLayers = bindingData.array.dims_count > 0 ? bindingData.array.dims[0] : 1;
            binding.isUnsizedArray = 
                ((bindingData.array.dims_count == 1) && (bindingData.array.dims[0] == 0));

            if (binding.type == ShaderBindingType::TEXTURE && bindingData.image.dim == SpvDim::SpvDimCube)
                binding.type = ShaderBindingType::CUBEMAP;

            m_slots[slot.glslSetIndex].bindings[binding.name] = binding;
        }

    }

    const auto pushConstantsData = GetPushConstantsReflection(shaderModule);

    for (UIndex64 pushConstantIndex = 0; pushConstantIndex < pushConstantsData.GetSize(); pushConstantIndex++) {
        const SpvReflectBlockVariable& pushConstantData = *(pushConstantsData[pushConstantIndex]);

        MaterialLayoutPushConstant pushConstantInfo{};
        pushConstantInfo.name = std::string(pushConstantData.name);
        pushConstantInfo.stage = stage; 
        pushConstantInfo.size = pushConstantData.size;
        pushConstantInfo.offset = pushConstantData.absolute_offset;

        m_pushConstants.Insert(pushConstantInfo);
    }
}

const std::unordered_map<OSK::UIndex64, OSK::GRAPHICS::MaterialLayoutSlot>& OSK::GRAPHICS::SpirvReflectionData::GetSlots() const {
    return m_slots;
}

std::span<const OSK::GRAPHICS::MaterialLayoutPushConstant> OSK::GRAPHICS::SpirvReflectionData::GetPushConstants() const {
    return m_pushConstants.GetFullSpan();
}

OSK::GRAPHICS::ShaderStage OSK::GRAPHICS::SpirvReflectionData::GetShaderStage() const {
    return m_stage;
}

void OSK::GRAPHICS::SpirvReflectionData::Log(IO::ILogger* logger) const {
    logger->DebugLog(std::format("Shader for {}.", ToString(m_stage)));

    logger->DebugLog("Slots:");

    for (const auto& [index, slot] : m_slots) {
        logger->DebugLog(std::format("\tIndex {}", index));

        for (const auto& [name, binding] : slot.bindings) {
            logger->DebugLog(std::format("\t\tBinding {}, type: {}", name, ToString(binding.type)));
        }
    }
}
