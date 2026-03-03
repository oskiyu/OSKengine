#pragma once

#include <string>
#include <optional>

#include "RgDependencyType.h"
#include "GpuBufferRange.h"
#include "GpuBarrierInfo.h"

#include "RgDependencyShaderBinding.h"
#include "ImageUuid.h"

namespace OSK::GRAPHICS {

	enum class RgBufferUsage {
		NONE,
		UNIFORM_BUFFER,
		STORAGE_BUFFER,
		VERTEX_BUFFER,
		INDEX_BUFFER,
		TRANSFER_SRC,
		TRANSFER_DST
	};


	/// @brief Declaración mínima
	/// de dependencia de un buffer
	/// necesaria para poder
	/// crear dicho buffer.
	struct PartialRgBufferDependency {

		/// @brief ID único del buffer.
		GdrBufferUuid bufferRefId;

		/// @brief Uso que se le va a dar.
		RgBufferUsage usage{};

	};

	/// @brief Dependencia de un buffer dentro
	/// de un render-pass:
	/// Indica un buffer que se va a usar dentro
	/// de un render-pass (y cómo se va a usar).
	struct RgBufferDependency {

		/// @brief ID único del buffer.
		GdrBufferUuid bufferRefId;

		/// @brief Uso que se le va a dar.
		RgBufferUsage usage{};

		/// @brief Tipo de dependencia.
		RgDependencyType type = RgDependencyType::READ_WRITE;

		/// @brief Stage en el que se usará.
		GpuCommandStage stage = GpuCommandStage::NONE;

		/// @brief Rango usado.
		GpuBufferRange range{};

		/// @brief Slot y binding en el que se usará.
		RgDependencyShaderBinding shaderBinding{};

		/// @brief Índice dentro del array.
		UIndex32 arrayIndex = 0;

		/// @brief Si está presente, indica en qué
		/// frame in flight se usará.
		std::optional<UIndex16> frameInFlight;

	};

}
