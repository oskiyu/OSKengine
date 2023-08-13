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
		~MaterialSlotVk();

		void SetUniformBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>);

		void SetGpuImages(
			const std::string& binding, 
			std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT>) override;

		void SetStorageBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>) override;

		void SetStorageImages(
			const std::string& binding, 
			std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT>) override;

		void SetAccelerationStructures(
			const std::string& binding, 
			std::span<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT>) override;

		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(UIndex32 index) const;

		void SetDebugName(const std::string& name) override;

	private:

		std::unordered_map<std::string, UIndex32> bindingsLocations;

		DynamicArray<VkDescriptorSet> descriptorSets;
		DynamicArray<DynamicArray<VkWriteDescriptorSet>> bindings;

		DynamicArray<UniquePtr<VkDescriptorBufferInfo>> bufferInfos;
		DynamicArray<UniquePtr<VkDescriptorImageInfo>> imageInfos;
		DynamicArray<UniquePtr<VkWriteDescriptorSetAccelerationStructureKHR>> accelerationStructureInfos;

		LinkedList<VkAccelerationStructureKHR> accelerationStructures;

		UniquePtr<DescriptorPoolVk> pool;
		UniquePtr<DescriptorLayoutVk> descLayout;

	};

}
