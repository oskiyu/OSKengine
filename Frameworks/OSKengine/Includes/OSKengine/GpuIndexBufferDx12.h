#pragma once

#include "IGpuindexBuffer.h"

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuIndexBufferDx12 : public IGpuIndexBuffer {

	public:

		GpuIndexBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

		void SetView(TSize numberOfIndices);
		const D3D12_INDEX_BUFFER_VIEW* GetView() const;

	private:

		D3D12_INDEX_BUFFER_VIEW view;

	};

}
