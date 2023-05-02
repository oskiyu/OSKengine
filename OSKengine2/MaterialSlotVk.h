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

		MaterialSlotVk(const std::string& name, const MaterialLayout* layout);
		~MaterialSlotVk();

		void SetUniformBuffers(
			const std::string& binding, 
			const GpuBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]);

		void SetGpuImages(
			const std::string& binding, 
			const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]) override;

		void SetStorageBuffers(
			const std::string& binding, 
			const GpuBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) override;

		void SetStorageImages(
			const std::string& binding, 
			const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]) override;

		void SetAccelerationStructures(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) override;

		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(TSize index) const;

		void SetDebugName(const std::string& name) override;

	private:

		HashMap<std::string, TSize> bindingsLocations;

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
