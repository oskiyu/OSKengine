#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "OldFontChar.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

namespace OSK {

	//Una fuente para el renderizado de texto.
	struct OSKAPI_CALL OldFont {

		//Carácteres de la fuente.
		std::map<char, OldFontChar> Characters;

		//Tamaño de la fuente.
		int32_t Size;

		//True si la fuente ha sido cargada y está lista para usarse.
		bool IsLoaded = false;

	};

}