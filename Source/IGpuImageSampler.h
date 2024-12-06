#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "GpuImageSamplerDesc.h"

namespace OSK::GRAPHICS {

	/// @brief Indica qué filtros y transformaciones aplicar
	/// al leer una imagen.
	class OSKAPI_CALL IGpuImageSampler {

	public:

		virtual ~IGpuImageSampler() = default;
		OSK_DEFINE_AS(IGpuImageSampler);

		const GpuImageSamplerDesc& GetInfo() const;

	protected:

		explicit IGpuImageSampler(const GpuImageSamplerDesc& info);

	private:

		GpuImageSamplerDesc m_info{};

	};

}
