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
	/// Debe definirse el layout del material, indicando qu� slots (y bindings) ser�n usados en los shaders.
	/// 
	/// @note Tiene la propiedad del graphics pipeline.
	/// </summary>
	class OSKAPI_CALL Material {

	public:

		using PipelineKey = DynamicArray<Format>;

		/// <summary>
		/// Crea un nuevo material.
		/// </summary>
		/// <param name="pipelineInfo">Informaci�n de las caracter�sticas del graphics pipeline.</param>
		/// <param name="layout">Layout del material. @note Este material ser� el due�o del layout.</param>
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
		/// Informaci�n para la generaci�n de los graphics pipelines.
		/// </summary>
		PipelineCreateInfo pipelineInfo{};

		UniquePtr<MaterialLayout> layout;

		DynamicArray<Pair<PipelineKey, UniquePtr<GRAPHICS::IGraphicsPipeline>>> graphicsPipelines;
		UniquePtr<GRAPHICS::IRaytracingPipeline> rtPipeline = nullptr;
		UniquePtr<GRAPHICS::IComputePipeline> computePipeline = nullptr;

		/// <summary>
		/// Informaci�n del v�rtice para generar los pipelines.
		/// </summary>
		const VertexInfo vertexInfo{};

		std::string name = "UNNAMED";

		MaterialType materialType;

	};

}
