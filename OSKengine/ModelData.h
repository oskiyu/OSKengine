#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

#include "Vertex.h"
#include "GPUDataBuffer.h"

namespace OSK {

	/// <summary>
	/// Informaci�n de un modelo.
	/// Contiene los buffers de los v�rtices y los �ndices.
	/// </summary>
	struct OSKAPI_CALL ModelData {

	public:

		/// <summary>
		/// Enlaza los v�rtices y los �ndices del modelo.
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
		/// V�rtices del modelo.
		/// </summary>
		GPUDataBuffer VertexBuffer;

		/// <summary>
		/// �ndices del modelo.
		/// </summary>
		GPUDataBuffer IndexBuffer;

		/// <summary>
		/// N�mero de �ndices.
		/// </summary>
		size_t IndicesCount = 0;

	};

}