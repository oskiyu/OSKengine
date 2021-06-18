#pragma once

#include "ECS.h"

#include <functional>

#include "KeyboardState.h"
#include "MouseState.h"

namespace OSK {

	/// <summary>
	/// Componente que permite a la entidad reaccionar al input del jugador.
	/// </summary>
	class InputComponent : public Component {
				
	public:

		OSK_COMPONENT(InputComponent)

		/// <summary>
		/// Devuelve la función que corresponde al input event dado.
		/// La función está definida en este componente.
		/// Esta función se puede definir.
		/// </summary>
		/// <param name="eventName">Nombre del evento.</param>
		/// <returns>Función del evento.</returns>
		OSKAPI_CALL std::function<void(deltaTime_t)>& GetInputFunction(const std::string& eventName);

		/// <summary>
		/// Devuelve la función que corresponde al input event dado.
		/// La función está definida en este componente.
		/// Esta función se puede definir.
		/// </summary>
		/// <param name="eventName">Nombre del evento.</param>
		/// <returns>Función del evento.</returns>
		OSKAPI_CALL std::function<void()>& GetOneTimeInputFunction(const std::string& eventName);
		
		/// <summary>
		/// Devuelve la función que corresponde al input event dado.
		/// La función está definida en este componente.
		/// Esta función se puede definir.
		/// </summary>
		/// <param name="eventName">Nombre del evento.</param>
		/// <returns>Función del evento.</returns>
		OSKAPI_CALL std::function<void(deltaTime_t, float)>& GetAxisInputFunction(const std::string& eventName);

		/// <summary>
		/// Ejecuta la función del evento de input dado.
		/// </summary>
		/// <param name="eventName">Nombre del evento.</param>
		/// <param name="deltaTime">Delta.</param>
		OSKAPI_CALL void ExecuteInputFunction(const std::string& eventName, deltaTime_t deltaTime);

		/// <summary>
		/// Ejecuta la función del evento de input dado.
		/// </summary>
		/// <param name="eventName">Nombre del evento.</param>
		OSKAPI_CALL void ExecuteOneTimeInputFunction(const std::string& eventName);

		/// <summary>
		/// Ejecuta la función del evento de input dado.
		/// </summary>
		/// <param name="eventName">Nombre del evento.</param>
		/// <param name="deltaTime">Delta.</param>
		OSKAPI_CALL void ExecuteAxisInputFunction(const std::string& eventName, deltaTime_t deltaTime, float axis);

	private:

		/// <summary>
		/// Eventos de input.
		/// </summary>
		std::unordered_map<std::string, std::function<void(deltaTime_t)>> inputEvents;

		/// <summary>
		/// Eventos de input de una sola vez.
		/// </summary>
		std::unordered_map<std::string, std::function<void()>> oneTimeInputEvents;

		/// <summary>
		/// Eventos de input de eje.
		/// </summary>
		std::unordered_map<std::string, std::function<void(deltaTime_t, float)>> axisInputEvents;

	};

}
