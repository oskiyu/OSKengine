#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "FontChar.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

namespace OSK {

	//Una fuente para el renderizado de texto.
	struct Font {

		//Car�cteres de la fuente.
		std::map<char, FontChar> Characters;

		//Tama�o de la fuente.
		int32_t Size;

		//True si la fuente ha sido cargada y est� lista para usarse.
		bool IsLoaded = false;

	};

}