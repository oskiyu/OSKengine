#pragma once

#include "IGpuDataBuffer.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un uniform buffer contiene información arbitraria a la que se puede
	/// acceder a la hora de renderizar.
	/// </summary>
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
