#pragma once

#include "IGpuDataBuffer.h"

namespace OSK {

	class OSKAPI_CALL IGpuIndexBuffer : public GpuDataBuffer {

	public:

		virtual ~IGpuIndexBuffer() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuIndexBuffer, T> {
			return (T*)this;
		}

	};

}
