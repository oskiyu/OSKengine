#pragma once

#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "ApiCall.h"
#include "HashMap.hpp"
#include "PipelineCreateInfo.h"
#include "MaterialLayout.h"
#include "VertexInfo.h"

#include "IGraphicsPipeline.h"
#include "IRaytracingPipeline.h"
#include "IComputePipeline.h"
#include "IMeshPipeline.h"

#include "MaterialType.h"

#include <compare>

namespace OSK::GRAPHICS {

	class MaterialInstance;
	struct PipelineCreateInfo;
	class IRenderpass;


	struct PipelineKey {

		bool operator==(const PipelineKey& other) const {
			if (other.colorFormats.GetSize() != colorFormats.GetSize())
				return false;

			if (other.depthFormat != depthFormat)
				return false;

			for (UIndex64 i = 0; i < colorFormats.GetSize(); i++)
				if (other.colorFormats[i] != colorFormats[i])
					return false;

			return true;
		}

		DynamicArray<Format> colorFormats;
		Format depthFormat{};

	};
	
	/// @brief Un material define el comportamiento del renderizador con un objeto en concreto.
	/// Debe definirse el layout del material, indicando qu� slots (y bindings) ser�n usados en los shaders,
	/// adem�s de los propios shaders.
	class OSKAPI_CALL Material {

	public:

		/// @brief Crea un nuevo material.
		/// @param pipelineInfo Configuraci�n de los shaders.
		/// @param layout Layout de los slots y bindings.
		/// @param vertexInfo Informaci�n del tipo de v�rtice usado, en caso de que se trate de un material gr�fico.
		/// @param type Tipo de material.
		/// 
		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		Material(const GRAPHICS::PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo, MaterialType type);

		/// @brief Establece el nombre del material.
		/// @param name Nombre.
		void SetName(const std::string& name);

		/// @brief Devuelve el layout del material.
		/// @return Layout del material.
		const MaterialLayout* GetLayout() const;


		/// @brief Obtiene el graphics pipeline para su uso de renderizado
		/// sobre im�genes con los formatos dados.
		/// @param properties Formatos de las im�genes de salida.
		/// @return Pipeline.
		/// 
		/// @pre El material debe ser un material gr�fico.
		/// 
		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		const IGraphicsPipeline* GetGraphicsPipeline(const PipelineKey& properties) const;

		/// @brief Obtiene el mesh pipeline para su uso de renderizado
		/// sobre im�genes con los formatos dados.
		/// @param properties Formatos de las im�genes de salida.
		/// @return Pipeline.
		/// 
		/// @pre El material debe ser un material gr�fico.
		/// 
		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		const IMeshPipeline* GetMeshPipeline(const PipelineKey& properties) const;

		/// @brief Obtiene el raytracing pipeline.
		/// @return Pipeline de trazado de rayos. Ser� null si el material
		/// no es de tipo trazado de rayos.
		/// 
		/// @pre El material debe ser un material de trazado de rayos.
		const IRaytracingPipeline* GetRaytracingPipeline() const;

		/// @brief Obtiene el raytracing pipeline.
		/// @return Pipeline de trazado de rayos. Ser� null si el material
		/// no es de tipo computaci�n.
		/// 
		/// @pre El material debe ser un material de computaci�n.
		const IComputePipeline* GetComputePipeline() const;
		

		/// @brief Crea y devuelve una nueva instancia de este material.
		/// @return Nueva instancia del material.
		OwnedPtr<MaterialInstance> CreateInstance();

		/// @brief Devuelve el tipo de material.
		/// @return Tipo de material.
		MaterialType GetMaterialType() const;


		const PipelineCreateInfo& GetInfo() const;
		std::string_view GetName() const;


		/// @brief Recarga los shaders del material.
		/// @warning No usar directamente, hacerlo mediante MaterialSystem.
		/// 
		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		void _Reload();

	private:

		/// @brief Inicializa los pipelines.
		/// 
		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		void InitializePipelines();

		/// @brief Informaci�n para la generaci�n de los graphics pipelines.
		PipelineCreateInfo pipelineInfo{};

		/// @brief Layout.
		UniquePtr<MaterialLayout> layout;

		/// @brief Graphics pipelines, una por cada combinaci�n de formatos de salida.
		mutable DynamicArray<UniquePtr<GRAPHICS::IGraphicsPipeline>> graphicsPipelines;
		/// @brief Formatos de los pipelines almacenados.
		mutable DynamicArray<PipelineKey> graphicsPipelinesKeys;

		/// @brief Mesh pipelines, una por cada combinaci�n de formatos de salida.
		mutable DynamicArray<UniquePtr<GRAPHICS::IMeshPipeline>> m_meshPipelines;
		/// @brief Formatos de los pipelines almacenados.
		mutable DynamicArray<PipelineKey> m_meshPipelinesKeys;

		UniquePtr<GRAPHICS::IRaytracingPipeline> rtPipeline = nullptr;
		UniquePtr<GRAPHICS::IComputePipeline> computePipeline = nullptr;

		/// @brief Informaci�n del v�rtice para generar los pipelines.
		const VertexInfo vertexInfo{};

		std::string name = "UNNAMED";

		MaterialType materialType;

	};

}
