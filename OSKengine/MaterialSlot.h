#pragma once

#include "MaterialSlotType.h"
#include "DescriptorSet.h"
#include "Texture.h"

#include "UniformBuffer.h"

namespace OSK {

	class MaterialSlotPool;
	class Material;

	/// <summary>
	/// Un material slot contiene información sobre buffers y texturas, que se utilizan con una periocidad similar.
	/// Equivalente a un descriptor set.
	/// </summary>
	class OSKAPI_CALL MaterialSlot {

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
		void SetBuffer(uint32_t binding, const UniformBuffer& ubo);
		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetBuffer(const std::string& bindingName, const UniformBuffer& ubo);

		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetDynamicBuffer(uint32_t binding, const UniformBuffer& ubo);
		/// <summary>
		/// Establece el buffer para el binding dado.
		/// 
		/// No llama a FlushUpdate.
		/// </summary>
		void SetDynamicBuffer(const std::string& bindingName, const UniformBuffer& ubo);

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

	private:

		uint32_t index = 0;
		MaterialSlotPool* pool = nullptr;
		Material* material = nullptr;
		bool hasBeenSet = false;
		std::vector<UniformBuffer> buffers;

	};

}
