#pragma once

#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "OSKmacros.h"
#include "HashMap.hpp"
#include "PipelineCreateInfo.h"
#include "MaterialLayout.h"
#include "VertexInfo.h"

namespace OSK::GRAPHICS {

	class MaterialInstance;
	class IGraphicsPipeline;
	struct PipelineCreateInfo;
	class IRenderpass;

	/// <summary>
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	/// Debe definirse el layout del material, indicando qué slots (y bindings) serán usados en los shaders.
	/// 
	/// @note Tiene la propiedad de los graphics pipelines (uno por cada renderpass registrado).
	/// </summary>
	class OSKAPI_CALL Material {

	public:

		/// <summary>
		/// Crea un nuevo material.
		/// </summary>
		/// <param name="pipelineInfo">Información de las características del graphics pipeline.</param>
		/// <param name="layout">Layout del material. @note Este material será el dueño del layout.</param>
		Material(const GRAPHICS::PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo);
		~Material();

		const MaterialLayout* GetLayout() const;

		/// <summary>
		/// Obtiene el graphics pipeline asignado al renderpass dado.
		/// </summary>
		const IGraphicsPipeline* GetGraphicsPipeline(const IRenderpass* ownerRenderpass) const;
		
		/// <summary>
		/// Registra el renderpass, creando una nueva graphics pipeline asignada a dicho renderpass.
		/// </summary>
		void RegisterRenderpass(const IRenderpass* renderpass);
		/// <summary>
		/// Elimina el graphics pipeline asignado a dicho renderpass.
		/// </summary>
		void UnregisterRenderpass(const IRenderpass* renderpass);

		/// <summary>
		/// Crea y devuelve una nueva instancia de este material.
		/// </summary>
		OwnedPtr<MaterialInstance> CreateInstance();

	private:

		/// <summary>
		/// Información para la generación de los graphics pipelines.
		/// </summary>
		PipelineCreateInfo pipelineInfo{};

		UniquePtr<MaterialLayout> layout;
		HashMap<const GRAPHICS::IRenderpass*, OwnedPtr<GRAPHICS::IGraphicsPipeline>> graphicsPipelines;

		/// <summary>
		/// Información del vértice para generar los pipelines.
		/// </summary>
		const VertexInfo vertexInfo;

	};

}
