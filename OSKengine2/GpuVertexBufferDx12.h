#pragma once

#include "IGpuVertexBuffer.h"

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuVertexBufferDx12 : public IGpuVertexBuffer {

	public:

		GpuVertexBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo);

		void SetView(TSize vertexSize, TSize numberOfVertices);

		/// <summary>
		/// Describe la estructura interna del buffer de vértices.
		/// </summary>
		D3D12_VERTEX_BUFFER_VIEW GetView() const;

	private:

		/// <summary>
		/// Describe la estructura interna del buffer de vértices.
		/// </summary>
		D3D12_VERTEX_BUFFER_VIEW view;

	};

}
