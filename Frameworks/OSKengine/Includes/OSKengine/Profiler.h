#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ProfilingUnit.h"
#include <vector>

namespace OSK {

	/// <summary>
	/// Clase que almacna unidades de profiling y puede mostrar sus valores por Logger.
	/// Para mostrar un mensaje distinto, crear una clase ahijada.
	/// </summary>
	class OSKAPI_CALL Profiler {

	public:

		/// <summary>
		/// A�ade una unidad de profiling.
		/// </summary>
		/// <param name="unit">Unidad a a�adir.</param>
		void AddProfilingUnit(ProfilingUnit* unit);

		/// <summary>
		/// Quita una unidad de profiling.
		/// </summary>
		/// <param name="unit">Unidad a quitar.</param>
		void RemoveProfilingUnit(ProfilingUnit* unit);

		/// <summary>
		/// Muestra la informaci�n de las unidades a trav�s del logger.
		/// </summary>
		virtual void ShowData();

	private:

		/// <summary>
		/// Unidades de profiling.
		/// </summary>
		std::vector<ProfilingUnit*> Units{};

	};

}
