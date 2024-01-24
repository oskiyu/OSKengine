#pragma once

#include "IMaterialSlot.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"
#include "DescriptorLayoutVk.h"
#include "DescriptorPoolVk.h"
#include "LinkedList.hpp"
#include "HashMap.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL MaterialSlotVk final : public IMaterialSlot {

	public:

		/// @throws DescriptorPoolCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws DescriptorLayoutCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws MaterialSlotCreationException Si hay algún error durante la creación del material slot.
		MaterialSlotVk(const std::string& name, const MaterialLayout* layout);
		~MaterialSlotVk() override;

		void SetUniformBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex);

		void SetGpuImages(
			const std::string& binding, 
			std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetStorageBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetStorageImages(
			const std::string& binding, 
			std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetAccelerationStructures(
			const std::string& binding, 
			std::span<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(UIndex32 index) const;

		void SetDebugName(const std::string& name) override;

	private:

		std::unordered_map<std::string, UIndex32, StringHasher, std::equal_to<>> m_bindingsLocations;


		/// @brief Un descriptor set por recurso en vuelo.
		DynamicArray<VkDescriptorSet> m_descriptorSets;
		
		/// @brief 
		DynamicArray<DynamicArray<VkWriteDescriptorSet>> m_bindings;

		DynamicArray<UniquePtr<VkDescriptorBufferInfo>> m_bufferInfos;
		DynamicArray<UniquePtr<VkDescriptorImageInfo>> m_imageInfos;
		DynamicArray<UniquePtr<VkWriteDescriptorSetAccelerationStructureKHR>> m_accelerationStructureInfos;

		LinkedList<VkAccelerationStructureKHR> m_accelerationStructures;

		UniquePtr<DescriptorPoolVk> m_pool;
		UniquePtr<DescriptorLayoutVk> m_descLayout;

	};

}
