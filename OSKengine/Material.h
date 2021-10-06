#pragma once

#include "MaterialPipelineInfo.h"
#include "Renderpass.h"
#include "GraphicsPipeline.h"
#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "MaterialSlotPool.h"
#include "OwnedPtr.h"

#include <unordered_map>

namespace OSK {

	class RenderAPI;
	class MaterialSystem;

	/// <summary>
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	/// También define el layout de los shaders.
	/// Contiene los graphics pipelines (con sus shaders) y los slots de las instancias de este material.
	/// </summary>
	class OSKAPI_CALL Material {

		friend class MaterialSystem;

	public:

		/// <summary>
		/// Crea un material vacío.
		/// </summary>
		/// <param name="type">Identificador del material.</param>
		/// <param name="owner">Sistemas de materiales del renderizador.</param>
		Material(MaterialPipelineTypeId type, MaterialSystem* owner);

		~Material();

		/// <summary>
		/// Establece el renderizador.
		/// </summary>
		void SetRenderer(RenderAPI* renderer);

		/// <summary>
		/// Establece la configuración de los pipelines que se irán creando durante la ejecución del juego.
		/// </summary>
		void SetPipelineSettings(const MaterialPipelineCreateInfo& info);

		/// <summary>
		/// Crea y devuelve una nueva instancia de este material.
		/// </summary>
		OwnedPtr<MaterialInstance> CreateInstance();

		/// <summary>
		/// Registra un nuevo renderpass, para crear un pipeline que le corresponda.
		/// </summary>
		void RegisterRenderpass(VULKAN::Renderpass* renderpass);
		/// <summary>
		/// Elimina el pipeline correspondiente al renderpass.
		/// </summary>
		void UnregisterRenderpass(VULKAN::Renderpass* renderpass);

		/// <summary>
		/// Devuelve el pipeline correspondiente al renderpass.
		/// </summary>
		GraphicsPipeline* GetGraphicsPipeline(VULKAN::Renderpass* renderpass) const;
		/// <summary>
		/// Devuelve la información con la que se crean los pipelines.
		/// </summary>
		MaterialPipelineCreateInfo GetMaterialGraphicsPipelineInfo() const;

		/// <summary>
		/// Devuelve un material slot pool que tenga al menos un hueco libre para el tipo dado.
		/// Si no existe, se crea uno nuevo.
		/// </summary>
		MaterialSlotPool* GetNextMaterialSlotPool(MaterialSlotTypeId type);

	private:

		MaterialPipelineCreateInfo pipelineInfo;
		std::vector<VULKAN::Renderpass*> renderpassesToRegister;
		std::unordered_map<VULKAN::Renderpass*, OwnedPtr<GraphicsPipeline>> graphicPipelines;

		std::unordered_map<MaterialSlotTypeId, std::list<OwnedPtr<MaterialSlotPool>>> materialSlotPools;

		RenderAPI* renderer = nullptr;
		MaterialSystem* owner = nullptr;
		MaterialPipelineTypeId pipelineType;

	};

}
