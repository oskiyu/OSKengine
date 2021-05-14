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
		 int useFxaa = 1;

		 /// <summary>
		 /// Resolución en X de la pantalla.
		 /// </summary>
		 int screenSizeX;

		 /// <summary>
		 /// Resolución en Y de la pantalla.
		 /// </summary>
		 int screenSizeY;

		 /// <summary>
		 /// % de resolución.
		 /// </summary>
		 float resolutionMultiplier;

	};

}
