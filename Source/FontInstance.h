#pragma once

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "HashMap.hpp"
#include "FontCharacter.h"
#include "Vector2.hpp"
#include "Sprite.h"
#include <string>

namespace OSK::ASSETS {

	/// @brief Representa una fuente que ha sido generada con un tamaño específico.
	/// Cada tamaño de fuente tendrá su propia imagen cargada.
	struct FontInstance {

		FontInstance() = default;
		OSK_DISABLE_COPY(FontInstance);
		OSK_DEFAULT_MOVE_OPERATOR(FontInstance);

		/// @brief Imagen en la GPU.
		/// @note Nunca será null.
		UniquePtr<GRAPHICS::GpuImage> image;

		/// @brief Sprite para el renderizado 2D.
		UniquePtr<GRAPHICS::Sprite> sprite;

		/// @brief Información de los sprites del carácter.
		std::unordered_map<char, FontCharacter> characters;

		/// @brief Tamaño de fuente de esta instancia.
		/// @note En píxeles.
		USize32 fontSize = 0;

		/// @brief Calcula el tamaño que tendrá este texto si es renderizado
		/// con esta instancia de la fuente.
		/// @param string Texto.
		/// @return Tamaño, en píxeles.
		Vector2f GetTextSize(std::string_view string) const;

	};

}
