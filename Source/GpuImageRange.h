#pragma once

#include "NumericTypes.h"
#include "GpuImageSamplerDesc.h"
#include "GpuBarrierInfo.h"


namespace OSK::GRAPHICS {

	/// @brief Describe un rango de una imagen,
	/// incluyendo capas, niveles de mip y canales.
	struct GpuImageRange {

		/// @brief �ndice de la primera capa que ser� afectada por el barrier.
		/// @pre Si la imagen NO es array, debe ser 0.
		UIndex32 baseLayer = 0;

		/// @brief N�mero de capas del array afectadas por el barrier.
		USize32 numLayers = ALL_IMAGE_LAYERS;

		/// @brief Nivel m�s bajo de mip que ser� afectado por el barrier.
		UIndex32 baseMipLevel = 0;
		/// @brief N�mero de niveles de mip que ser�n afectados por el barrier.
		USize32 numMipLevels = ALL_MIP_LEVELS;


		/// @brief Canal(es) afectados por el barrier.
		SampledChannel channel = SampledChannel::COLOR;

	};

}
