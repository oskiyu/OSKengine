#pragma once

#include "ApiCall.h"
#include "HashMap.hpp"
#include "DynamicArray.hpp"

#include "GlobalVertexBuffer.h"
#include "GlobalIndexBuffer.h"

#include "VertexAttributes.h"
#include "RenderGraphArgs.h"
#include "GpuGeometryDesc.h"
#include "GpuBufferRange.h"

namespace OSK::GRAPHICS {

	class IRenderer;
	class RgBufferManager;


	struct VertexAttributeEntry {
		std::string name;
		GdrBufferUuid bufferRef = GdrBufferUuid::CreateEmpty();
		USize32 sizeOfAttrib = 0;
		TIndexSize indexInsideTable = 0;
	};

	/// @brief Maneja la geometría almacenada
	/// en un render-graph:
	/// - Descripciones de buffers de atributos de vértices.
	/// - Descripciones de buffers de índices.
	/// 
	/// Funciona bajo un modelo de renderizado con
	/// vertex pulling, por lo que no tiene un
	/// buffer de vértices tradicional.
	/// 
	/// Los vértices se dividen en buffers (uno
	/// por cada tipo de atributo soportado).
	/// 
	/// Los vértices no tienen por qué tener todos
	/// los mismos atributos.
	class OSKAPI_CALL RgGeometryManager {

	public:

		/// @brief Inicializa el manejador.
		/// @param renderer Renderizador.
		/// @param manager Manager de buffers del render-graph.
		/// @param maxVertices Número máximo de vértices a soportar.
		explicit RgGeometryManager(
			IRenderer* renderer,
			RgBufferManager* manager,
			USize64 maxVertices);


		/// @brief Registra un nuevo tipo de atributo de vértice.
		/// @param uuid Indentificadro del atributo.
		/// @param name Nombre del atributo.
		/// @param attributeSize Tamaño de la estructura
		/// @param bufferUuid UUID del buffer que será creado.
		/// del atributo, en bytes.
		/// @note Si ya existía un atributo con
		/// el identificador @p uuid, se sobreescribirá.
		/// 
		/// @pre No se deben haber registrado los
		/// buffers mediante `RegisterUnifiedGeometryBuffers`
		/// (si se llama a esta función después de `RegisterUnifiedGeometryBuffers`,
		/// será ignorada).
		void RegisterVertexAttribute(
			VertexAttribUuid uuid,
			std::string name,
			USize32 attributeSize,
			GdrBufferUuid bufferUuid);

		/// @brief Crea y registra los buffers de vértices,
		/// de índices y de atributos.
		/// 
		/// @pre Deben haberse registrado todos los atributos
		/// a usar mediante `RegisterVertexAttribute(...)`.
		void RegisterUnifiedGeometryBuffers();

		void PartiallyRegisterUnifiedGeometryBuffers();

		void PrepareUnifiedGeometryBuffersReferences();

		/// @brief Registra una geomatría en concreto,
		/// incorporándola a los buffers correspondientes.
		/// @param geometry Geometría a registrar.
		/// @return Información sobre el registro.
		GpuGeometryEntry RegisterGeometry(const GpuGeometryDesc& geometry);

		std::span<const VertexAttributeEntry> GetRegisteredAttribs() const;
		std::span<const GdrBufferUuid> GetRegisteredAttribsBufferUuids() const;

		RgBufferRef GetAttribsIndexes() const;
		RgBufferRef GetIndexesBuffer() const;

	private:

		/// @brief Registra los índices de una geometría.
		/// @param indexes Índices a registrar.
		/// @return Información sobre el registro.
		/// 
		/// @pre Los índices deben haber sido desplazados respecto al
		/// primer índice disponible.
		GlobalIndexBufferRange RegisterGeometryIndexes(std::span<const TIndexSize> indexes);


		/// @brief Tipos de atributos de los vértices.
		std::unordered_map<VertexAttribUuid, VertexAttributeEntry> m_attributesInfo;

		/// @brief UUIDs de los buffers donde se guardarán los atributos.
		DynamicArray<GdrBufferUuid> m_attributesBufferUuids{};

		/// @brief Lista con todos los atributos.
		DynamicArray<VertexAttributeEntry> m_attribs;

		/// @brief Manager de los buffers por cada tipo de atributo.
		std::unordered_map<VertexAttribUuid, GlobalVertexBufferManager> m_verticesBufferManager;


		// -- BUFFER IDS -- //

		/// @brief ID del buffer de índices.
		RgBufferRef m_indexesBufferId;

		/// @brief ID del buffer con los índices de los atributos.
		RgBufferRef m_vertexAttribsIndexesBufferId;


		// -- MANAGERS -- //

		GlobalVertexBufferManager m_attributesIndicesBufferManager;
		GlobalIndexBufferManager m_indexesBufferManager;

		IRenderer* m_renderer{};
		RgBufferManager* m_bufferManager{};

		USize64 m_maxVertices = 0;

	};

}
