#pragma once

#include "Uuid.h"

namespace OSK::GRAPHICS {

	/// @brief Identificador de una imagen
	/// usada en un render-graph.
	using GdrImageUuid = BaseUuid<class GdrImage>;

	/// @brief Identificador de un buffer
	/// usado en un render-graph.
	using GdrBufferUuid = BaseUuid<class GdrBuffer>;

}

OSK_DEFINE_UUID_HASH(OSK::GRAPHICS::GdrBufferUuid);
OSK_DEFINE_UUID_HASH(OSK::GRAPHICS::GdrImageUuid);
