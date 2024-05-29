#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"


namespace OSK {

	/// @brief Clase que es capaz de medir el tiempo transcurrido
	/// entre dos instantes.
	class OSKAPI_CALL StopWatch {

	public:

		/// @brief Estados en los que se puede encontrar un StopWatch.
		enum class State {
			/// @brief El StopWatch está parado.
			STOPPED = 0,
			/// @brief El StopWatch está midiendo el tiempo transcurrido.
			RUNNING
		};

	public:

		/// @brief Comienza el conteo de tiempo.
		/// @pre El estado anterior a esta llamada debe ser State::STOPPED.
		/// @post El estado después de esta llamada será State::RUNNING.
		void Start();

		/// @brief Detiene el conteo de tiempo.
		/// @pre El estado anterior a esta llamada debe ser State::RUNNING.
		/// @post El estado después de esta llamada será State::STOPPED.
		void Stop();


		/// @pre Debe haberse llamado al menos una vez a Start().
		/// @return Tiempo (en segundos) medidos por el StopWatch:
		/// - Si el estado actual es State::RUNNING, entonces devuelve el tiempo transcurrido
		/// desde la última llamada a Start().
		/// - Si el estado actual es State::STOPPED, entonces devuelve el tiempo transcurrido
		/// entre la última llamada a Start() y la última llamada a Stop().
		TDeltaTime GetElapsedTime() const;

		/// @return Estado actual del StopWatch.
		State GetCurrentState() const;

	private:

		TDeltaTime m_startTime = 0.0f;
		TDeltaTime m_endTime = 0.0f;

		State m_state = State::STOPPED;

	};

}
