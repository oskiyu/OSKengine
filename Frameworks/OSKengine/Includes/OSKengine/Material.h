#pragma once

#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "OSKmacros.h"
#include "HashMap.hpp"
#include "PipelineCreateInfo.h"
#include "MaterialLayout.h"
#include "VertexInfo.h"
#include "IRaytracingPipeline.h"

namespace OSK::GRAPHICS {

	class MaterialInstance;
	class IGraphicsPipeline;
	struct PipelineCreateInfo;
	class IRenderpass;

	/// <summary>
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	/// Debe definirse el layout del material, indicando qué slots (y bindings) serán usados en los shaders.
	/// 
	/// @note Tiene la propiedad del graphics pipeline.
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
		const IGraphicsPipeline* GetGraphicsPipeline() const;

		/// <summary>
		/// Obtiene el raytracing pipeline asignado al renderpass dado.
		/// </summary>
		const IRaytracingPipeline* GetRaytracingPipeline() const;
		
		/// <summary>
		/// Crea y devuelve una nueva instancia de este material.
		/// </summary>
		OwnedPtr<MaterialInstance> CreateInstance();

		/// <summary>
		/// Devuelve true si es un material de renderizado por trazado de rayos,
		/// y devuelve false si es un pipeline de rasterizado.
		/// </summary>
		bool IsRaytracing() const;

	private:

		/// <summary>
		/// Información para la generación de los graphics pipelines.
		/// </summary>
		PipelineCreateInfo pipelineInfo{};

		UniquePtr<MaterialLayout> layout;

		UniquePtr<GRAPHICS::IGraphicsPipeline> graphicsPipeline = nullptr;
		UniquePtr<GRAPHICS::IRaytracingPipeline> rtPipeline = nullptr;

		/// <summary>
		/// Información del vértice para generar los pipelines.
		/// </summary>
		const VertexInfo vertexInfo;

	};

}
