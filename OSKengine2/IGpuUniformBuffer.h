#pragma once

#include "IGpuDataBuffer.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un uniform buffer contiene información arbitraria a la que se puede
	/// acceder a la hora de renderizar.
	/// Es read-only.
	/// </summary>
	class OSKAPI_CALL IGpuUniformBuffer : public GpuDataBuffer {

	public:

		OSK_DEFINE_AS(IGpuUniformBuffer);

		virtual ~IGpuUniformBuffer() = default;

	protected:
	
		IGpuUniformBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
