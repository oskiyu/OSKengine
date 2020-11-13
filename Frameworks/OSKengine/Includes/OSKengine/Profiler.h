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

		//Añade una unidad de profiling.
		//	<unit>: unidad a añadir.
		void AddProfilingUnit(ProfilingUnit* unit);

		//Quita una unidad de profiling.
		//	<unit>: unidad a quitar.
		void RemoveProfilingUnit(ProfilingUnit* unit);

		//Muestra la información de las unidades a través del logger.
		virtual void ShowData();

	private:

		std::vector<ProfilingUnit*> Units{};

	};

}
