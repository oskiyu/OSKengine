#pragma once

#include "IMaterialSlot.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"
#include "DescriptorLayoutVulkan.h"
#include "DescriptorPoolVulkan.h"
#include "LinkedList.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL MaterialSlotVulkan : public IMaterialSlot {

	public:

		MaterialSlotVulkan(const std::string& name, const MaterialLayout* layout);
		~MaterialSlotVulkan();

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void SetGpuImage(const std::string& binding, const GpuImage* image) override;
		void SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) override;
		void SetStorageImage(const std::string& binding, const GpuImage* image) override; 
		void SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) override;
		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(TSize index) const;

	private:

		DynamicArray<VkDescriptorSet> descriptorSets;
		DynamicArray<DynamicArray<VkWriteDescriptorSet>> bindings;

		DynamicArray<OwnedPtr<VkDescriptorBufferInfo>> bufferInfos;
		DynamicArray<OwnedPtr<VkDescriptorImageInfo>> imageInfos;
		DynamicArray<OwnedPtr<VkWriteDescriptorSetAccelerationStructureKHR>> accelerationStructureInfos;

		LinkedList<VkAccelerationStructureKHR> accelerationStructures;

		UniquePtr<DescriptorPoolVulkan> pool;
		UniquePtr<DescriptorLayoutVulkan> descLayout;

	};

}
