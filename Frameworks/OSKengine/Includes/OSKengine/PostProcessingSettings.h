#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Ajustes de postprocesamiento.
	/// </summary>
	struct OSKAPI_CALL PostProcessingSettings_t {

		 /// <summary>
		 /// True (1) si usa antailasing FXAA.
		 /// </summary>
		 int UseFXAA = 1;

		 /// <summary>
		 /// Resoluci�n en X de la pantalla.
		 /// </summary>
		 int ScreenSizeX;

		 /// <summary>
		 /// Resoluci�n en Y de la pantalla.
		 /// </summary>
		 int ScreenSizeY;

	};

}
