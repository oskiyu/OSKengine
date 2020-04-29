#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Indica qué hacer cuando el texto sobrepasa el límite de renderizado.
	enum class TextRenderingLimit {

		//No haer nada.
		DO_NOTHING,

		//Hacer newline.
		NEW_LINE,

		//Mover texto.
		MOVE_TEXT

	};

}
