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
#include <unordered_set>
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

		void SetGpuImage(std::string_view binding, const IGpuImageView& view, UIndex32 arrayIndex) override;
		void SetStorageImage(std::string_view binding, const IGpuImageView& view, UIndex32 arrayIndex) override;

		void SetUniformBuffer(std::string_view binding, const GpuBuffer& buffer, const GpuBufferRange& range, UIndex32 arrayIndex) override;
		void SetStorageBuffer( std::string_view binding, const GpuBuffer& buffer, const GpuBufferRange& range, UIndex32 arrayIndex) override;

		void SetAccelerationStructure(std::string_view binding, const ITopLevelAccelerationStructure& accelerationStructure, UIndex32 arrayIndex) override;

		void FlushUpdate() override;

		void SetDebugName(const std::string& name) override;

		VkDescriptorSet GetDescriptorSet() const;

	private:

		static OwnedPtr<VkDescriptorBufferInfo> GetDescriptorBufferInfo(const GpuMemorySubblockVk& subblock, const GpuBufferRange& range);
		static OwnedPtr<VkDescriptorImageInfo> GetDescriptorImageInfo(const IGpuImageView& view, VkImageLayout layout);

		struct BindingVk {
			/// @brief Mapa ID en el array del shader -> descriptor write.
			std::unordered_map<UIndex32, VkWriteDescriptorSet> descriptorWrites{};

			/// @brief Mapa ID en el array del shader -> buffer info.
			std::unordered_map<UIndex32, UniquePtr<VkDescriptorBufferInfo>> bufferInfos{};
			/// @brief Mapa ID en el array del shader -> image info.
			std::unordered_map<UIndex32, UniquePtr<VkDescriptorImageInfo>> imageInfos{};
			/// @brief Mapa ID en el array del shader -> as info.
			std::unordered_map<UIndex32, UniquePtr<VkWriteDescriptorSetAccelerationStructureKHR>> asInfos{};

			/// @brief Indica los recursos que deben ser actualizados.
			std::unordered_set<UIndex32> dirtyDescriptorWrites{};

		};

		/// @brief Para evitar tener que alojar memoria todos
		/// los frames.
		DynamicArray<VkWriteDescriptorSet> m_descriptorWrites{};

		VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
		
		/// @brief Mapa ID del binding -> datos del binding.
		std::unordered_map<UIndex32, BindingVk>  m_bindings;

		DynamicArray<VkAccelerationStructureKHR> m_accelerationStructures;

		UniquePtr<DescriptorPoolVk> m_pool;
		UniquePtr<DescriptorLayoutVk> m_descLayout;

	};

}
