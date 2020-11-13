#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Tipo de output del renderizado.
	enum class OSKAPI_CALL RenderOutput {
		SCENE,
		NORMALS,
		POSITION
	};

}
