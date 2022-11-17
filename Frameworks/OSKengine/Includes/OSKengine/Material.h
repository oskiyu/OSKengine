#pragma once

#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "OSKmacros.h"
#include "HashMap.hpp"
#include "PipelineCreateInfo.h"
#include "MaterialLayout.h"
#include "VertexInfo.h"

#include "IGraphicsPipeline.h"
#include "IRaytracingPipeline.h"
#include "IComputePipeline.h"

#include "MaterialType.h"

namespace OSK::GRAPHICS {

	class MaterialInstance;
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

		using PipelineKey = DynamicArray<Format>;

		/// <summary>
		/// Crea un nuevo material.
		/// </summary>
		/// <param name="pipelineInfo">Información de las características del graphics pipeline.</param>
		/// <param name="layout">Layout del material. @note Este material será el dueño del layout.</param>
		Material(const GRAPHICS::PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo, MaterialType type);

		void SetName(const std::string& name);

		const MaterialLayout* GetLayout() const;

		/// <summary>
		/// Obtiene el graphics pipeline con las propiedades dadas.
		/// </summary>
		const IGraphicsPipeline* GetGraphicsPipeline(const PipelineKey& properties);

		/// <summary>
		/// Obtiene el raytracing pipeline.
		/// </summary>
		const IRaytracingPipeline* GetRaytracingPipeline() const;

		const IComputePipeline* GetComputePipeline() const;
		
		/// <summary>
		/// Crea y devuelve una nueva instancia de este material.
		/// </summary>
		OwnedPtr<MaterialInstance> CreateInstance();

		MaterialType GetMaterialType() const;

	private:

		/// <summary>
		/// Información para la generación de los graphics pipelines.
		/// </summary>
		PipelineCreateInfo pipelineInfo{};

		UniquePtr<MaterialLayout> layout;

		DynamicArray<Pair<PipelineKey, UniquePtr<GRAPHICS::IGraphicsPipeline>>> graphicsPipelines;
		UniquePtr<GRAPHICS::IRaytracingPipeline> rtPipeline = nullptr;
		UniquePtr<GRAPHICS::IComputePipeline> computePipeline = nullptr;

		/// <summary>
		/// Información del vértice para generar los pipelines.
		/// </summary>
		const VertexInfo vertexInfo{};

		std::string name = "UNNAMED";

		MaterialType materialType;

	};

}
