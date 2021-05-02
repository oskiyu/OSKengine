#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <string>

namespace OSK {

	/// <summary>
	/// Clase que representa una unidad de profiling.
	/// Maniene información sobre el rendimiento de una parte del motor / juego.
	/// Funciona como un cronómetro.
	/// </summary>
	class OSKAPI_CALL ProfilingUnit {

	public:

		/// <summary>
		/// Crea una unidad vacía.
		/// </summary>
		ProfilingUnit();

		/// <summary>
		/// Crea una unidad.
		/// </summary>
		/// <param name="name">Nombre de la unidad.</param>
		ProfilingUnit(const std::string& name);

		/// <summary>
		/// Comienza el análisis de la unidad.
		/// </summary>
		void Start();

		/// <summary>
		/// Termina el análisis de la unidad.
		/// </summary>
		void End();

		/// <summary>
		/// Obtiene el valor de la unidad.
		/// </summary>
		/// <returns>Valor.</returns>
		deltaTime_t GetValue() const;

		/// <summary>
		/// Nombre de la unidad.
		/// </summary>
		std::string Name = "UNKNOWN";

	private:

		/// <summary>
		/// Inicio.
		/// </summary>
		deltaTime_t startTime = 0;

		/// <summary>
		/// Fin.
		/// </summary>
		deltaTime_t endTime = 0;

		/// <summary>
		/// Duración anterior.
		/// </summary>
		deltaTime_t lastDuration = 0;

	};

}
