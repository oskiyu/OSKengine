#pragma once

#include "OSKmacros.h"

#include <string>

namespace OSK {

	struct Version;

	/// <summary>
	/// Esta clase representa la clase principal de un juego de OSKengine.
	/// No es estrictamente necesaria para desarrollar con OSKengine, pero
	/// es un buen punto de partida.
	/// Ofrece una abstracci�n m�nima.
	/// 
	/// Deben sobreescribirse las funciones de CreateWindow()
	/// y SetupEngine() de manera obligatoria.
	/// </summary>
	class OSKAPI_CALL IGame {

	public:

		virtual ~IGame() = default;

		/// <summary>
		/// Funci�n ejecutada al cargar el juego.
		/// Se ejecuta dfespu�s de haber cargado todos
		/// los sistemas del juego.
		/// </summary>
		virtual void OnCreate();

		/// <summary>
		/// Funci�n que se ejecuta cada frame.
		/// Para hacer actualizaci�n del estado del juego.
		/// 
		/// Es preferible usar ECS para manejar actualizaciones
		/// del estado del juego.
		/// </summary>
		/// <param name="deltaTime">Tiempo que ha pasado desde el �ltimo frame,
		/// en segundos.</param>
		virtual void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Funci�n que se ejecuta al salir del juego.
		/// </summary>
		virtual void OnExit();


		/// <summary>
		/// Inicia la ejecuci�n del juego.
		/// </summary>
		void Run();

		/// <summary>
		/// Fuerza el shutdown del juego.
		/// </summary>
		void Exit();

		/// <summary>
		/// Devuelve el n�mero de frames en el �ltimo segundo.
		/// </summary>
		TSize GetFps() const;

	protected:

		/// <summary>
		/// Debe crear la ventana del Engine.
		/// </summary>
		virtual void CreateWindow() = 0;

		/// <summary>
		/// Debe inicializar el renderizador del Engine.
		/// </summary>
		virtual void SetupEngine() = 0;

	private:

		TDeltaTime deltaTime = 1.0f;

		TDeltaTime framerateCountTimer = 0.0f;
		TSize currentFps = 0;
		TSize frameCount = 0;

	};

}
