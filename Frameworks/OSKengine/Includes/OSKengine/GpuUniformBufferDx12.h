#pragma once

#include "IGpuUniformBuffer.h"

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuUniformBufferDx12 : public IGpuUniformBuffer {

	public:

		GpuUniformBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

		D3D12_CONSTANT_BUFFER_VIEW_DESC GetView() const;

	private:

		void SetView();

		D3D12_CONSTANT_BUFFER_VIEW_DESC view{};

	};

}
