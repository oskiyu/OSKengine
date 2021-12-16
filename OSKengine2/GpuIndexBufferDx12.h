#pragma once

#include "IGpuindexBuffer.h"

#include <d3d12.h>

namespace OSK {

	class OSKAPI_CALL GpuIndexBufferDx12 : public IGpuIndexBuffer {

	public:

		void SetView(TSize numberOfIndices);
		D3D12_INDEX_BUFFER_VIEW GetView() const;

	private:

		D3D12_INDEX_BUFFER_VIEW view;

	};

}
