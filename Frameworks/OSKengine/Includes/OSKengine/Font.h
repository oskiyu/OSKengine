#pragma once

#include <map>
#include "FontChar.h"

#include "MaterialInstance.h"

namespace OSK {

	/// <summary>
	/// Fuente para el renderizado de texto.
	/// </summary>
	struct OSKAPI_CALL Font {

		/// <summary>
		/// Espacios a los que equivale '\t'.
		/// </summary>
		const static int SPACES_PER_TAB = 4;
		
		/// <summary>
		/// Obtiene el tama�o de un texto que se renderice con esta fuente.
		/// </summary>
		/// <param name="texto">Texto.</param>
		/// <param name="size">Tama�o de la fuente.</param>
		/// <returns>Tama�o del texto.</returns>
		Vector2 GetTextSize(const std::string& texto, const Vector2& size) const;

		/// <summary>
		/// Caracteres de la fuente.
		/// </summary>
		std::map<char, FontChar> Characters;

		/// <summary>
		/// Tama�o de la fuente.
		/// </summary>
		uint32_t Size;

		/// <summary>
		/// Material de la fuente, para su renderizado.
		/// </summary>
		SharedPtr<MaterialInstance> FontMaterial;

	};

}