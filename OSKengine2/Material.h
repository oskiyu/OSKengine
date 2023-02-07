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

	
	/// @brief Un material define el comportamiento del renderizador con un objeto en concreto.
	/// Debe definirse el layout del material, indicando qué slots (y bindings) serán usados en los shaders,
	/// además de los propios shaders.
	class OSKAPI_CALL Material {

	public:

		using PipelineKey = DynamicArray<Format>;


		/// @brief Crea un nuevo material.
		/// @param pipelineInfo Configuración de los shaders.
		/// @param layout Layout de los slots y bindings.
		/// @param vertexInfo Información del tipo de vértice usado, en caso de que se trate de un material gráfico.
		/// @param type Tipo de material.
		Material(const GRAPHICS::PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo, MaterialType type);

		/// @brief Establece el nombre del material-
		/// @param name Nombre.
		void SetName(const std::string& name);

		/// @brief Devuelve el layout del material.
		/// @return Layout del material.
		const MaterialLayout* GetLayout() const;

		
		/// @brief Obtiene el graphics pipeline para su uso de renderizado
		/// sobre imágenes con los formatos dados.
		/// @param properties Formatos de las imágenes de salida.
		/// @return Pipeline.
		/// 
		/// @pre El material debe ser un material gráfico.
		const IGraphicsPipeline* GetGraphicsPipeline(const PipelineKey& properties) const;

		/// @brief Obtiene el raytracing pipeline.
		/// @return Pipeline de trazado de rayos. Será null si el material
		/// no es de tipo trazado de rayos.
		/// 
		/// @pre El material debe ser un material de trazado de rayos.
		const IRaytracingPipeline* GetRaytracingPipeline() const;

		/// @brief Obtiene el raytracing pipeline.
		/// @return Pipeline de trazado de rayos. Será null si el material
		/// no es de tipo computación.
		/// 
		/// @pre El material debe ser un material de computación.
		const IComputePipeline* GetComputePipeline() const;
		

		/// @brief Crea y devuelve una nueva instancia de este material.
		/// @return Nueva instancia del material.
		OwnedPtr<MaterialInstance> CreateInstance();

		/// @brief Devuelve el tipo de material.
		/// @return Tipo de material.
		MaterialType GetMaterialType() const;


		/// @brief Recarga los shaders del material.
		/// @warning No usar directamente, hacerlo mediante MaterialSystem.
		void _Reload();

	private:

		/// @brief Inicializa los pipelines.
		void InitializePipelines();

		/// @brief Información para la generación de los graphics pipelines.
		PipelineCreateInfo pipelineInfo{};

		/// @brief Layout.
		UniquePtr<MaterialLayout> layout;

		/// @brief Graphics pipelines, una por cada combinación de formatos de salida.
		mutable DynamicArray<UniquePtr<GRAPHICS::IGraphicsPipeline>> graphicsPipelines;
		/// @brief Formatos de los pipelines almacenados.
		mutable DynamicArray<PipelineKey> graphicsPipelinesKeys;

		UniquePtr<GRAPHICS::IRaytracingPipeline> rtPipeline = nullptr;
		UniquePtr<GRAPHICS::IComputePipeline> computePipeline = nullptr;

		/// @brief Información del vértice para generar los pipelines.
		const VertexInfo vertexInfo{};

		std::string name = "UNNAMED";

		MaterialType materialType;

	};

}
