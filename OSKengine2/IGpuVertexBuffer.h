#pragma once

#include "IGpuDataBuffer.h"

namespace OSK {

	class OSKAPI_CALL IGpuVertexBuffer : public GpuDataBuffer {

	public:

		virtual ~IGpuVertexBuffer() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuVertexBuffer, T> {
			return (T*)this;
		}

	private:

	};

}
