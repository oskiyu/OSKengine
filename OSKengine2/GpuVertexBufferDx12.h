#pragma once

#include "IGpuVertexBuffer.h"

#include <d3d12.h>

namespace OSK {

	class OSKAPI_CALL GpuVertexBufferDx12 : public IGpuVertexBuffer {

	public:

		void SetView(TSize vertexSize, TSize numberOfVertices);
		D3D12_VERTEX_BUFFER_VIEW GetView() const;

	private:

		D3D12_VERTEX_BUFFER_VIEW view;

	};

}