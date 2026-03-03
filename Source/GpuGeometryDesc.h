#pragma once

#include "ApiCall.h"
#include "Vertex.h"
#include "VertexAttributes.h"
#include "DynamicArray.hpp"

#include "GlobalIndexBuffer.h"
#include "GlobalVertexBuffer.h"

namespace OSK::GRAPHICS {


	/// @brief Información sobre
	/// en qué zonas de los buffers
	/// globales de geometría se
	/// ha almacenado una geometría
	/// en concreto.
	struct GpuGeometryEntry {

		GpuGeometryEntry() = default;

		explicit GpuGeometryEntry(
			GlobalIndexBufferRange indexesEntry,
			GlobalVertexBufferRange attribsIndexesEntry,
			std::unordered_map<VertexAttribUuid, GlobalVertexBufferRange>&& vertexEntriesPerAttrib)
			:
			indexesEntry(indexesEntry),
			attribsIndexesEntry(attribsIndexesEntry),
			vertexEntriesPerAttrib(std::move(vertexEntriesPerAttrib))
		{

		}

		/// @brief Índices de la geometría.
		GlobalIndexBufferRange indexesEntry{};

		/// @brief Estructuras con los índices de
		/// cada uno de los atributos.
		GlobalVertexBufferRange attribsIndexesEntry{};

		/// @brief Atributos de los vértices.
		std::unordered_map<VertexAttribUuid, GlobalVertexBufferRange> vertexEntriesPerAttrib{};

	};

	/// @brief Descripción en CPU de un modelo
	/// de geometría en GPU.
	class OSKAPI_CALL GpuGeometryDesc {

	public:

		explicit GpuGeometryDesc(const DynamicArray<TIndexSize>& indexes);

		const DynamicArray<TIndexSize>& GetIndexes() const;

		VerticesAttributesMaps& GetAttributesMap();
		const VerticesAttributesMaps& GetAttributesMap() const;

		GpuGeometryEntry entry = {};

	private:

		DynamicArray<TIndexSize> m_indexes;
		VerticesAttributesMaps m_attributes;
		
	};

}
