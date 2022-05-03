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
	/// Representa una fuente que ha sido generada con un tama�o espec�fico.
	/// Cada tama�o de fuente tendr� su propia imagen cargada.
	/// </summary>
	struct FontInstance {

		/// <summary>
		/// Imagen en la GPU.
		/// 
		/// @note Nunca ser� null.
		/// </summary>
		UniquePtr<GRAPHICS::GpuImage> image;

		/// <summary>
		/// Sprite para el renderizado 2D.
		/// </summary>
		UniquePtr<GRAPHICS::Sprite> sprite;

		/// <summary>
		/// Informaci�n de los sprites del car�cter.
		/// </summary>
		HashMap<char, FontCharacter> characters;

		/// <summary>
		/// Tamma�o de fuente de esta instancia.
		/// En p�xeles.
		/// </summary>
		TSize fontSize = 0;

		/// <summary>
		/// Calcula el tama�o que tendr� este texto si es renderizado
		/// con esta instancia de la fuente.
		/// </summary>
		/// <returns>Tama�o, en p�xeles.</returns>
		Vector2f GetTextSize(const std::string& string) const;

	};

}
