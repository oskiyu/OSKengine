#pragma once

#include <map>
#include "FontChar.h"

namespace OSK {

	//Fuente para el renderizado de texto.
	struct OSKAPI_CALL Font {

		//Espacios a los que equivale '\t'.
		const static int SPACES_PER_TAB = 4;
		//Obtiene el tamaño de un texto que se renderice con esta fuente.
		Vector2 GetTextSize(const std::string& texto, const Vector2& size) const;
		//Caracteres de la fuente.
		std::map<char, FontChar> Characters;
		//Tamaño de la fuente.
		uint32_t Size;

	};

}