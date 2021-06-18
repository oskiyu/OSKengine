#pragma once

#include "ECS.h"
#include "InputComponent.h"

#include <string>
#include "WindowAPI.h"

#include "InputEvent.h"
#include "OneTimeInputEvent.h"
#include "AxisInputEvent.h"

namespace OSK {

	/// <summary>
	/// Sistema que maneja el input del jugador.
	/// </summary>
	class OSKAPI_CALL InputSystem : public ECS::System {

	public:

		/// <summary>
		/// Establece la ventana del juego.
		/// Para 'escuchar' al input.
		/// </summary>
		/// <param name="window">Ventana.</param>
		void SetWindow(WindowAPI* window);

		/// <summary>
		/// Comprueba el input y ejecuta las funciones que tengan que ejecutarse. 
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void OnTick(deltaTime_t deltaTime) override;

		/// <summary>
		/// Obtiene el Signature (componentes necesarios de un objeto) del sistema.
		/// </summary>
		/// <returns>Signature.</returns>
		Signature GetSystemSignature() override;

		/// <summary>
		/// Devuelve el evento de input que tiene el nombre dado.
		/// </summary>
		/// <param name="e">Nombre del evento.</param>
		/// <returns>Evento.</returns>
		InputEvent& GetInputEvent(const std::string& e);

		/// <summary>
		/// Registra un nuevo evento de input.
		/// </summary>
		/// <param name="name">Nombre del evento.</param>
		void RegisterInputEvent(const std::string& name);

		/// <summary>
		/// Devuelve el evento de input de una sola vez que tiene el nombre dado.
		/// </summary>
		/// <param name="e">Nombre del evento.</param>
		/// <returns>Evento.</returns>
		OneTimeInputEvent& GetOneTimeInputEvent(const std::string& e);

		/// <summary>
		/// Registra un nuevo evento de input de una sola vez.
		/// </summary>
		/// <param name="name">Nombre del evento.</param>
		void RegisterOneTimeInputEvent(const std::string& name);

		/// <summary>
		/// Devuelve el evento de input de eje de que tiene el nombre dado.
		/// </summary>
		/// <param name="e">Nombre del evento.</param>
		/// <returns>Evento.</returns>
		AxisInputEvent& GetAxisInputEvent(const std::string& e);

		/// <summary>
		/// Registra un nuevo evento de input de eje.
		/// </summary>
		/// <param name="name">Nombre del evento.</param>
		void RegisterAxisInputEvent(const std::string& name);

	private:

		/// <summary>
		/// Ejecuta la función del evento dado.
		/// </summary>
		/// <param name="name">Nombre del evento.</param>
		/// <param name="deltaTime">Delta.</param>
		void executeFunction(std::string& name, deltaTime_t deltaTime);

		/// <summary>
		/// Ejecuta la función del evento de input de una sola vez dado.
		/// </summary>
		/// <param name="name">Nombre del evento.</param>
		void executeOneTimeFunction(std::string& name);

		/// <summary>
		/// Ejecuta la función del evento de input de una sola vez dado.
		/// </summary>
		/// <param name="name">Nombre del evento.</param>
		void executeAxisFunction(std::string& name, deltaTime_t deltaTime, float axis);

		/// <summary>
		/// Ventana.
		/// </summary>
		WindowAPI* window = nullptr;

		/// <summary>
		/// Estado del teclado en el tick anterior.
		/// </summary>
		KeyboardState oldKeyboardState;

		/// <summary>
		/// Estado del teclado.
		/// </summary>
		KeyboardState newKeyboardState;

		/// <summary>
		/// Estado del ratón en el tick anterior.
		/// </summary>
		MouseState oldMouseState;

		/// <summary>
		/// Estado del ratón.
		/// </summary>
		MouseState newMouseState;

		/// <summary>
		/// Map nombre de evento -> evento.
		/// </summary>
		std::unordered_map<std::string, InputEvent> events;

		/// <summary>
		/// Map nombre de evento -> evento.
		/// </summary>
		std::unordered_map<std::string, OneTimeInputEvent> oneTimeEvents;

		/// <summary>
		/// Map nombre de evento -> evento de eje.
		/// </summary>
		std::unordered_map<std::string, AxisInputEvent> axisEvents;

	};

}
