#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Indica qu� hacer cuando el texto sobrepasa el l�mite de renderizado.
	enum class TextRenderingLimit {

		//No haer nada.
		DO_NOTHING,

		//Hacer newline.
		NEW_LINE,

		//Mover texto.
		MOVE_TEXT

	};

}
