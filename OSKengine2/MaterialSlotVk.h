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

	class OSKAPI_CALL MaterialSlotVk : public IMaterialSlot {

	public:

		MaterialSlotVk(const std::string& name, const MaterialLayout* layout);
		~MaterialSlotVk();

		void SetUniformBuffer(
			const std::string& binding, 
			const IGpuUniformBuffer* buffer) override;

		void SetUniformBuffers(
			const std::string& binding, 
			const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]);

		void SetGpuImage(
			const std::string& binding,
			const IGpuImageView* image) override;

		void SetGpuImages(
			const std::string& binding, 
			const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]) override;

		void SetStorageBuffer(
			const std::string& binding, 
			const GpuDataBuffer* buffer) override;

		void SetStorageBuffers(
			const std::string& binding, 
			const GpuDataBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) override;

		void SetStorageImage(
			const std::string& binding, 
			const IGpuImageView* image) override;

		void SetStorageImages(
			const std::string& binding, 
			const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]) override;

		void SetAccelerationStructure(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure) override;

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
