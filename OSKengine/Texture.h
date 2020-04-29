#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Una textura.
	struct Texture {

	public:

		//OpenGL.
		OSK_INFO_DO_NOT_TOUCH
			textureCode_t ID;

		//Tama�o de la textura.
		Vector2 Size;

		//True si la textura ha sido cargada y est� lista para usarse.
		bool IsLoaded = false;

	};

}
