#pragma once

#include <map>
#include "FontChar.h"

namespace OSK {

	struct OSKAPI_CALL Font {

		//Espacios a los que equivale '\t'.
		const static int SPACES_PER_TAB = 4;

		Vector2 GetTextSize(const std::string& texto, const Vector2& size) const;

		std::map<char, FontChar> Characters;

		uint32_t Size;

	};

}