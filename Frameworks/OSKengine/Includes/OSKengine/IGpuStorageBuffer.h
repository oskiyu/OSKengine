#pragma once

#include "IGpuDataBuffer.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un buffer que contiene información read-write.
	/// Equivalente a un UAV buffer en DX12.
	/// </summary>
	class OSKAPI_CALL IGpuStorageBuffer : public GpuDataBuffer {

	public:

		OSK_DEFINE_AS(IGpuStorageBuffer);

	protected:

		IGpuStorageBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
