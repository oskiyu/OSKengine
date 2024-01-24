#pragma once

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "HashMap.hpp"
#include "FontCharacter.h"
#include "Vector2.hpp"
#include "Sprite.h"
#include <string>

namespace OSK::ASSETS {

	/// @brief Representa una fuente que ha sido generada con un tama�o espec�fico.
	/// Cada tama�o de fuente tendr� su propia imagen cargada.
	struct FontInstance {

		FontInstance() = default;
		OSK_DISABLE_COPY(FontInstance);
		OSK_DEFAULT_MOVE_OPERATOR(FontInstance);

		/// @brief Imagen en la GPU.
		/// @note Nunca ser� null.
		UniquePtr<GRAPHICS::GpuImage> image;

		/// @brief Sprite para el renderizado 2D.
		UniquePtr<GRAPHICS::Sprite> sprite;

		/// @brief Informaci�n de los sprites del car�cter.
		std::unordered_map<char, FontCharacter> characters;

		/// @brief Tama�o de fuente de esta instancia.
		/// @note En p�xeles.
		USize32 fontSize = 0;

		/// @brief Calcula el tama�o que tendr� este texto si es renderizado
		/// con esta instancia de la fuente.
		/// @param string Texto.
		/// @return Tama�o, en p�xeles.
		Vector2f GetTextSize(std::string_view string) const;

	};

}
