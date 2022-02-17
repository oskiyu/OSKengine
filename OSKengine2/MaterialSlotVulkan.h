#pragma once

#include "IMaterialSlot.h"
#include "DynamicArray.hpp"

struct VkDescriptorSet_T;
typedef VkDescriptorSet_T* VkDescriptorSet;

struct VkDescriptorBufferInfo_T;
typedef VkDescriptorBufferInfo_T* VkDescriptorBufferInfo;

namespace OSK {

	class OSKAPI_CALL MaterialSlotVulkan : public IMaterialSlot {

	public:

		MaterialSlotVulkan(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void FlushUpdate() override;

	private:

		void ClearGarabage();

		DynamicArray<VkDescriptorSet> descriptorSets;
		DynamicArray<DynamicArray<VkWriteDescriptorSet>> bindings;

		DynamicArray<OwnedPtr<VkDescriptorBufferInfo>> bufferInfos;

	};

}
