#pragma once

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "HashMap.hpp"
#include "FontCharacter.h"
#include "Vector2.hpp"
#include "Sprite.h"
#include <string>

namespace OSK::ASSETS {

	/// <summary>
	/// Representa una fuente que ha sido generada con un tamaño específico.
	/// Cada tamaño de fuente tendrá su propia imagen cargada.
	/// </summary>
	struct FontInstance {

		/// <summary>
		/// Imagen en la GPU.
		/// 
		/// @note Nunca será null.
		/// </summary>
		UniquePtr<GRAPHICS::GpuImage> image;

		/// <summary>
		/// Sprite para el renderizado 2D.
		/// </summary>
		UniquePtr<GRAPHICS::Sprite> sprite;

		/// <summary>
		/// Información de los sprites del carácter.
		/// </summary>
		HashMap<char, FontCharacter> characters;

		/// <summary>
		/// Tammaño de fuente de esta instancia.
		/// En píxeles.
		/// </summary>
		TSize fontSize = 0;

		/// <summary>
		/// Calcula el tamaño que tendrá este texto si es renderizado
		/// con esta instancia de la fuente.
		/// </summary>
		/// <returns>Tamaño, en píxeles.</returns>
		Vector2f GetTextSize(const std::string& string) const;

	};

}
