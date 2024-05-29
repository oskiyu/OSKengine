#pragma once

#include "IMaterialSlot.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"
#include "DescriptorLayoutVk.h"
#include "DescriptorPoolVk.h"
#include "LinkedList.hpp"
#include "ResourcesInFlight.h"

#include <unordered_map>
#include <array>

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuMemorySubblockVk;
	class IGpuImageView;


	class OSKAPI_CALL MaterialSlotVk final : public IMaterialSlot {

	public:

		/// @throws DescriptorPoolCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws DescriptorLayoutCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws MaterialSlotCreationException Si hay algún error durante la creación del material slot.
		MaterialSlotVk(const std::string& name, const MaterialLayout* layout);
		~MaterialSlotVk() override;

		void SetUniformBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetGpuImages(
			const std::string& binding, 
			std::span<const IGpuImageView*, MAX_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetStorageBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetStorageImages(
			const std::string& binding, 
			std::span<const IGpuImageView*, MAX_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void SetAccelerationStructures(
			const std::string& binding, 
			std::span<const ITopLevelAccelerationStructure*, MAX_RESOURCES_IN_FLIGHT>,
			UIndex32 arrayIndex) override;

		void FlushUpdate() override;

		VkDescriptorSet GetDescriptorSet(UIndex32 index) const;

		void SetDebugName(const std::string& name) override;

	private:

		static OwnedPtr<VkDescriptorBufferInfo> GetDescriptorBufferInfo(const GpuMemorySubblockVk& subblock);
		static OwnedPtr<VkDescriptorImageInfo> GetDescriptorImageInfo(const IGpuImageView& view, VkImageLayout layout);

		struct BindingVk {
			/// @brief Mapa ID en el array del shader -> descriptor write.
			std::unordered_map<UIndex32, VkWriteDescriptorSet> descriptorWrites{};
		};

		DynamicArray<VkWriteDescriptorSet> m_descriptorWrites{};

		/// @brief Un descriptor set por recurso en vuelo.
		std::array<VkDescriptorSet, MAX_RESOURCES_IN_FLIGHT> m_descriptorSets{};
		
		/// @brief Mapas ID del binding -> datos del binding.
		std::array<std::unordered_map<UIndex32, BindingVk>, MAX_RESOURCES_IN_FLIGHT> m_bindings;

		DynamicArray<UniquePtr<VkDescriptorBufferInfo>> m_bufferInfos;
		DynamicArray<UniquePtr<VkDescriptorImageInfo>> m_imageInfos;
		DynamicArray<UniquePtr<VkWriteDescriptorSetAccelerationStructureKHR>> m_accelerationStructureInfos;

		LinkedList<VkAccelerationStructureKHR> m_accelerationStructures;

		UniquePtr<DescriptorPoolVk> m_pool;
		UniquePtr<DescriptorLayoutVk> m_descLayout;

	};

}
