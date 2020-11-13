#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ProfilingUnit.h"
#include <vector>

namespace OSK {

	//Clase que almacna unidades de profiling y puede mostrar sus valores por Logger.
	//Para mostrar un mensaje distinto, crear una clase ahijada.
	class OSKAPI_CALL Profiler {

	public:

		//A�ade una unidad de profiling.
		//	<unit>: unidad a a�adir.
		void AddProfilingUnit(ProfilingUnit* unit);

		//Quita una unidad de profiling.
		//	<unit>: unidad a quitar.
		void RemoveProfilingUnit(ProfilingUnit* unit);

		//Muestra la informaci�n de las unidades a trav�s del logger.
		virtual void ShowData();

	private:

		std::vector<ProfilingUnit*> Units{};

	};

}
