#pragma once

#include "IMaterialSlot.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"
#include "DescriptorLayoutVulkan.h"
#include "DescriptorPoolVulkan.h"
#include "LinkedList.hpp"
#include "HashMap.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL MaterialSlotVulkan : public IMaterialSlot {

	public:

		MaterialSlotVulkan(const std::string& name, const MaterialLayout* layout);
		~MaterialSlotVulkan();

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void SetUniformBuffers(const std::string& binding, const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]);
		void SetGpuImage(const std::string& binding, const GpuImage* image, SampledChannel channel, SampledArrayType arrayType, TSize arrayLevel) override;
		void SetGpuImages(const std::string& binding, const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel, SampledArrayType arrayType, TSize arrayLevel) override;
		void SetStorageBuffer(const std::string& binding, const IGpuStorageBuffer* buffer) override;
		void SetStorageBuffers(const std::string& binding, const IGpuStorageBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetStorageImage(const std::string& binding, const GpuImage* image, SampledArrayType arrayType, TSize arrayLevel) override;
		void SetStorageImages(const std::string& binding, const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], SampledArrayType arrayType, TSize arrayLevel) override;
		void SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) override;
		void SetAccelerationStructures(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) override;
		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(TSize index) const;

	private:

		HashMap<std::string, TSize> bindingsLocations;

		DynamicArray<VkDescriptorSet> descriptorSets;
		DynamicArray<DynamicArray<VkWriteDescriptorSet>> bindings;

		DynamicArray<UniquePtr<VkDescriptorBufferInfo>> bufferInfos;
		DynamicArray<UniquePtr<VkDescriptorImageInfo>> imageInfos;
		DynamicArray<UniquePtr<VkWriteDescriptorSetAccelerationStructureKHR>> accelerationStructureInfos;

		LinkedList<VkAccelerationStructureKHR> accelerationStructures;

		UniquePtr<DescriptorPoolVulkan> pool;
		UniquePtr<DescriptorLayoutVulkan> descLayout;

	};

}
