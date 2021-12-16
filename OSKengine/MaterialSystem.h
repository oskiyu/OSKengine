#pragma once

#include "Material.h"
#include "MaterialBinding.h"
#include "MaterialSlotType.h"

#include <list>
#include <unordered_map>

namespace OSK {

	namespace VULKAN {
		class Renderpass;
	}

	/// <summary>
	/// Sistema que maneja los materiales del renerizador.
	/// </summary>
	class OSKAPI_CALL MaterialSystem {

	public:

		/// <summary>
		/// Crea el sistema.
		/// </summary>
		MaterialSystem(RenderAPI* renderer);
		~MaterialSystem();

		/// <summary>
		/// Crea un nuevo material.
		/// </summary>
		/// <param name="type">Identificador del material.</param>
		void RegisterMaterial(MaterialPipelineTypeId type);
		/// <summary>
		/// Devuelve un material dado su identificador.
		/// </summary>
		Material* GetMaterial(MaterialPipelineTypeId type);

		/// <summary>
		/// Registra un renderpass en todos los materiales.
		/// </summary>
		void RegisterRenderpass(VULKAN::Renderpass* renderpass);
		/// <summary>
		/// Elimina un renderpass en todos los materiales.
		/// </summary>
		void UnregisterRenderpass(VULKAN::Renderpass* renderpass);

		/// <summary>
		/// Establece el layout del slot de un material.
		/// </summary>
		/// <param name="mPipeline">Tipo de material.</param>
		/// <param name="type">Slot.</param>
		/// <param name="layout">Layout del slot.</param>
		void SetDescriptorLayout(MaterialPipelineTypeId mPipeline, MaterialSlotTypeId type, MaterialBindingLayout layout);
		/// <summary>
		/// Devuelve el descriptor layout de un slot.
		/// </summary>
		DescriptorLayout* GetDescriptorLayout(MaterialSlotTypeId type);

		/// <summary>
		/// Devuelve los descriptor layouts de un material.
		/// </summary>
		const std::vector<VkDescriptorSetLayout>& GetMaterialPipelineLayout(MaterialPipelineTypeId pipeline) const;

		uint32_t GetBindingFromName(MaterialSlotTypeId type, const std::string& name) const;

	private:

		std::unordered_map<MaterialPipelineTypeId, OwnedPtr<Material>> materials;
		std::list<VULKAN::Renderpass*> renderpasses;

		std::unordered_map<MaterialPipelineTypeId, std::vector<VkDescriptorSetLayout>> pipelinesLayouts;

		std::unordered_map<MaterialSlotTypeId, OwnedPtr<DescriptorLayout>> descriptorLayouts;
		RenderAPI* renderer = nullptr;

	};

}
