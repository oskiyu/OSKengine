#pragma once

#include "Component.h"
#include "DynamicArray.hpp"
#include "AssetRef.h"
#include "Texture.h"
#include "HashMap.hpp"
#include "IGpuImageView.h"
#include "ImageUuid.h"

namespace OSK::ECS {

	/// @brief Referencia las imágenes/texturas
	/// de una geometría.
	struct GdrTexturedComponent {

		OSK_COMPONENT("OSK::GdrTexturedComponent");

		/// @brief Identificadores de las imágenes.
		GRAPHICS::GdrImageUuid colorImageUuid;
		GRAPHICS::GdrImageUuid normalImageUuid;

		/// @brief Referencias a las texturas cargadas.
		/// Para que no sean eliminadas accidentalmente.
		DynamicArray<ASSETS::AssetRef<ASSETS::Texture>> texturesRefs;

	};

}
