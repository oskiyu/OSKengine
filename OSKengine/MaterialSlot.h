#pragma once

#include "MaterialSlotType.h"
#include "DescriptorSet.h"
#include "Texture.h"
#include "MaterialPipelineInfo.h"

#include "UniformBuffer.h"

namespace OSK {

	class MaterialSlotPool;
	class Material;

	class OSKAPI_CALL MaterialSlotBindings {

	public:

		void SetTexture(uint32_t binding, Texture* texture) {
			SetTexture(binding, texture->image);
		}
		void SetTexture(uint32_t binding, SharedPtr<OSK::VULKAN::GpuImage> image) {
			textures[binding] = image;
		}

		void SetBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo) {
			buffers[binding] = ubo;
		}
		void SetDynamicBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo) {
			dynamicBuffers[binding] = ubo;
		}

		bool operator==(const MaterialSlotBindings& other) {
			for (const auto& i : textures) {
				auto it = other.textures.find(i.first);

				if (it == other.textures.end() || (*it).second.GetPointer() != i.second.GetPointer())
					return false;
			}

			for (const auto& i : buffers) {
				auto it = other.buffers.find(i.first);

				if (it == other.buffers.end() || (*it).second.GetPointer() != i.second.GetPointer())
					return false;
			}

			for (const auto& i : dynamicBuffers) {
				auto it = other.dynamicBuffers.find(i.first);

				if (it == other.dynamicBuffers.end() || (*it).second.GetPointer() != i.second.GetPointer())
					return false;
			}

			return false;
		}

		bool operator!=(const MaterialSlotBindings& other) {
			return !operator==(other);
		}

	private:

		std::unordered_map<uint32_t, SharedPtr<OSK::VULKAN::GpuImage>> textures;
		std::unordered_map<uint32_t, SharedPtr<UniformBuffer>> buffers;
		std::unordered_map<uint32_t, SharedPtr<UniformBuffer>> dynamicBuffers;

	};

	class OSKAPI_CALL MaterialSlot {

	public:

		MaterialSlot() {}
		MaterialSlot(Material* owner, MaterialSlotTypeId type) : material(owner), slotType(type) {

		}

		void SetTexture(uint32_t binding, Texture* texture);
		void SetTexture(uint32_t binding, SharedPtr<OSK::VULKAN::GpuImage> image);
		void SetTexture(const std::string& bindingName, Texture* texture);
		void SetTexture(const std::string& bindingName, SharedPtr<OSK::VULKAN::GpuImage> image);
		

		void SetBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo);
		void SetBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo);

		void SetDynamicBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo);
		void SetDynamicBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo);

		void FlushUpdate();

		MaterialSlotHandler GetHandler() const;

		MaterialSlotBindings bindings;

	private:

		MaterialSlotTypeId slotType;
		MaterialSlotHandler handler = 0;
		Material* material = nullptr;

	};

	/// <summary>
	/// Un material slot contiene información sobre buffers y texturas, que se utilizan con una periocidad similar.
	/// Equivalente a un descriptor set.
	/// </summary>
	class OSKAPI_CALL MaterialSlotData {

		friend class MaterialSlotPool;

	public:

		/// <summary>
		/// Tipo de slot.
		/// </summary>
		MaterialSlotTypeId slotType;

		/// <summary>
		/// Establece la textura para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetTexture(uint32_t binding, Texture* texture);
		/// <summary>
		/// Establece la textura para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetTexture(uint32_t binding, SharedPtr<OSK::VULKAN::GpuImage> image);
		/// <summary>
		/// Establece la textura para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetTexture(uint32_t binding, OSK::VULKAN::GpuImage* image);
		/// <summary>
		/// Establece la textura para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetTexture(const std::string& bindingName, Texture* texture);
		/// <summary>
		/// Establece la textura para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetTexture(const std::string& bindingName, SharedPtr<OSK::VULKAN::GpuImage> image);
		/// <summary>
		/// Establece la textura para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetTexture(const std::string& bindingName, OSK::VULKAN::GpuImage* image);

		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo);
		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo);

		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetDynamicBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo);
		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetDynamicBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo);

		/// <summary>
		/// Devuelve el descriptor set.
		/// </summary>
		DescriptorSet* GetDescriptorSet() const;
		/// <summary>
		/// True si el descriptor set ha sido inicializado.
		/// </summary>
		bool HasBeenSet() const;

		/// <summary>
		/// Actualiza el descriptor set.
		/// </summary>
		void FlushUpdate();


		/// <summary>
		/// Establece el descriptor set como el que se va a usar a partir de ahora en un CommandBuffer.
		/// </summary>
		/// <param name="commandBuffer">CommandBuffer.</param>
		/// <param name="pipeline">Pipeline con el que se está renderizando.</param>
		/// <param name="iteration">Qué VulkanDescriptorSets se va a usar.</param>
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration);

		/// <summary>
		/// Establece el descriptor set como el que se va a usar a partir de ahora en un CommandBuffer.
		/// </summary>
		/// <param name="commandBuffer">CommandBuffer.</param>
		/// <param name="pipeline">Pipeline con el que se está renderizando.</param>
		/// <param name="iteration">Qué VulkanDescriptorSets se va a usar.</param>
		/// <param name="dynamicOffset">Offset de la entidad del dynamic UBO.</param>
		/// <param name="alignment">Alignment del dynamic ubo.</param>
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration, uint32_t dynamicOffset, uint32_t alignment);

		MaterialSlotBindings bindings;

	private:

		uint32_t index = 0;
		MaterialSlotPool* pool = nullptr;
		Material* material = nullptr;
		bool hasBeenSet = false;

		std::vector<SharedPtr<UniformBuffer>> buffers;
		std::vector<VULKAN::GpuImage*> textures;

	};

}
