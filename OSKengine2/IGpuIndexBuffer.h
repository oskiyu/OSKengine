#pragma once

#include "IGpuDataBuffer.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un index buffer contiene los índices para el renderizado de un modelo 2D o 3D.
	/// </summary>
	class OSKAPI_CALL IGpuIndexBuffer : public GpuDataBuffer {

	public:

		IGpuIndexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

		virtual ~IGpuIndexBuffer() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuIndexBuffer, T> {
			return (T*)this;
		}

	};

}
