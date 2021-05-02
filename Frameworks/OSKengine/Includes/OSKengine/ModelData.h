#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

#include "Vertex.h"
#include "GPUDataBuffer.h"

namespace OSK {

	/// <summary>
	/// Información de un modelo.
	/// Contiene los buffers de los vértices y los índices.
	/// </summary>
	struct OSKAPI_CALL ModelData {

	public:

		/// <summary>
		/// Enlaza los vértices y los índices del modelo.
		/// Para su uso en el renderizado.
		/// </summary>
		/// <param name="commandBuffer">Command buffer.</param>
		void Bind(VkCommandBuffer commandBuffer) const;

		/// <summary>
		/// Renderiza el modelo, individualmente.
		/// Debe llamarse a Bind antes.
		/// </summary>
		/// <param name="commandBuffer">Command buffer.</param>
		void Draw(VkCommandBuffer commandBuffer) const;

		/// <summary>
		/// Vértices del modelo.
		/// </summary>
		GPUDataBuffer VertexBuffer;

		/// <summary>
		/// Índices del modelo.
		/// </summary>
		GPUDataBuffer IndexBuffer;

		/// <summary>
		/// Número de índices.
		/// </summary>
		size_t IndicesCount = 0;

	};

}