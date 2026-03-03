#pragma once

#include <string>
#include "RgDependencyType.h"
#include "Format.h"
#include "GpuImageUsage.h"
#include "GpuBarrierInfo.h"
#include "GpuImageRange.h"
#include "GpuImageViewConfig.h"
#include "ImageUuid.h"
#include "RgDependencyShaderBinding.h"
#include <optional>

namespace OSK::GRAPHICS {
	

	/// @brief Dependencia mínima
	/// necesaria para poder 
	/// crear una imagen.
	struct PartialRgImageDependency {

		/// @brief ID único de la imagen.
		GdrImageUuid imageRefId;

		/// @brief Uso que se le va a dar en el render-pass.
		GpuExclusiveImageUsage usage = GpuExclusiveImageUsage::UNKNOWN;

	};


	/// @brief Dependencia de una imagen dentro
	/// de un render-pass:
	/// Indica una imagen que se va a usar dentro
	/// de un render-pass (y cómo se va a usar).
	struct RgImageDependency {

		/// @brief ID único de la imagen.
		GdrImageUuid imageRefId;

		/// @brief Tipo de dependencia.
		RgDependencyType type = RgDependencyType::READ_WRITE;

		/// @brief Uso que se le va a dar en el render-pass.
		GpuExclusiveImageUsage usage = GpuExclusiveImageUsage::UNKNOWN;

		/// @brief Stage en el que se usará.
		GpuCommandStage stage = GpuCommandStage::NONE;

		/// @brief Rango de la imagen usada.
		GpuImageRange range{};

		/// @brief Vista de la imagen.
		GpuImageViewConfig viewConfig{};

		/// @brief Sampler de la imagen.
		GpuImageSamplerDesc samplerDesc{};

		/// @brief Slot y binding en el que se usará.
		RgDependencyShaderBinding shaderBinding{};

		/// @brief Índice dentro del array.
		UIndex32 arrayIndex = 0;

		/// @brief En caso de que sea una imagen de
		/// renderizado (de color o de profundidad),
		/// si se debe limpiar o no.
		bool clearIfTargetImage = false;

		/// @brief Si está presente, indica en qué
		/// frame in flight se usará.
		std::optional<UIndex16> frameInFlight;

	};

}
