#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Textura en forma de cubemap.
	//Skybox.
	struct OSKAPI_CALL OldSkybox {

	public:

		//OpenGL.
		OSK_INFO_DO_NOT_TOUCH
			textureCode_t ID;

		//True si el skybox ha sido cargado y está listo para usarse.
		bool IsLoaded = false;

	};

}