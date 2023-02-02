#pragma once

#include "IGpuDataBuffer.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un buffer que contiene informaci�n read-write.
	/// Equivalente a un UAV buffer en DX12.
	/// </summary>
	class OSKAPI_CALL IGpuStorageBuffer : public GpuDataBuffer {

	public:

		OSK_DEFINE_AS(IGpuStorageBuffer);

		virtual ~IGpuStorageBuffer() = default;

	protected:

		IGpuStorageBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
