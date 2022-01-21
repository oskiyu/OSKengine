#pragma once

#include "IGpuDataBuffer.h"

namespace OSK {

	/// <summary>
	/// Un vertex buffer es un buffer de memoria en la GPU que almacena los
	/// vértices de un modelo 3D o un sprite.	
	/// </summary>
	class OSKAPI_CALL IGpuVertexBuffer : public GpuDataBuffer {

	public:

		IGpuVertexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

		virtual ~IGpuVertexBuffer() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuVertexBuffer, T> {
			return (T*)this;
		}

	private:

	};

}
