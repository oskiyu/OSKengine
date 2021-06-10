#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

#include "Vertex.h"
#include "GpuDataBuffer.h"
#include "SharedPtr.hpp"

namespace OSK {

	/// <summary>
	/// Información de un modelo.
	/// Contiene los buffers de los vértices y los índices.
	/// </summary>
	class OSKAPI_CALL ModelData {

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
		SharedPtr<GpuDataBuffer> vertexBuffer;

		/// <summary>
		/// Índices del modelo.
		/// </summary>
		SharedPtr<GpuDataBuffer> indexBuffer;

		/// <summary>
		/// Número de índices.
		/// </summary>
		size_t indicesCount = 0;

	};

}