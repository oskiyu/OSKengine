#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <string>

namespace OSK {

	//Clase que representa una unidad de profiling.
	//Maniene informaci�n sobre el rendimiento de una parte del motor / juego.
	//Funciona como un cron�metro.
	class OSKAPI_CALL ProfilingUnit {

	public:

		//Crea una unidad vac�a.
		ProfilingUnit();

		//Crea una unidad.
		//	<name>: nombre de la unidad.
		ProfilingUnit(const std::string& name);

		//Comienza el an�lisis de la unidad.
		void Start();

		//Termina el an�lisis de la unidad.
		void End();

		//Obtiene el valor de la unidad.
		deltaTime_t GetValue() const;

		//Nombre de la unidad.
		std::string Name = "UNKNOWN";

	private:

		deltaTime_t startTime = 0;
		deltaTime_t endTime = 0;
		deltaTime_t lastDuration = 0;

	};

}
