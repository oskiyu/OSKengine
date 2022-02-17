#pragma once

#include "IGpuDataBuffer.h"

namespace OSK {

	class OSKAPI_CALL IGpuUniformBuffer : public GpuDataBuffer {

	public:

		template <typename T> T* As() const requires std::is_base_of_v<IGpuUniformBuffer, T> {
			return (T*)this;
		}

	protected:

		IGpuUniformBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	private:

	};

}
