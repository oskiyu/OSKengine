#pragma once

#include "IMaterialSlot.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class DescriptorPoolVulkan;
	class DescriptorLayoutVulkan;

	class OSKAPI_CALL MaterialSlotVulkan : public IMaterialSlot {

	public:

		MaterialSlotVulkan(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void SetGpuImage(const std::string& binding, const GpuImage* image) override;
		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(TSize index) const;

	private:

		DynamicArray<VkDescriptorSet> descriptorSets;
		DynamicArray<DynamicArray<VkWriteDescriptorSet>> bindings;

		DynamicArray<OwnedPtr<VkDescriptorBufferInfo>> bufferInfos;
		DynamicArray<OwnedPtr<VkDescriptorImageInfo>> imageInfos;

		UniquePtr<DescriptorPoolVulkan> pool;
		UniquePtr<DescriptorLayoutVulkan> descLayout;

	};

}
