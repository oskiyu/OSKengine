#pragma once

#include "OSKmacros.h"

#include <string>

namespace OSK {

	struct Version;

	/// <summary>
	/// Esta clase representa la clase principal de un juego de OSKengine.
	/// No es estrictamente necesaria para desarrollar con OSKengine, pero
	/// es un buen punto de partida.
	/// Ofrece una abstracción mínima.
	/// 
	/// Deben sobreescribirse las funciones de CreateWindow()
	/// y SetupEngine() de manera obligatoria.
	/// </summary>
	class OSKAPI_CALL IGame {

	public:

		virtual ~IGame() = default;

		/// <summary>
		/// Función ejecutada al cargar el juego.
		/// Se ejecuta dfespués de haber cargado todos
		/// los sistemas del juego.
		/// </summary>
		virtual void OnCreate();

		/// <summary>
		/// Función que se ejecuta cada frame.
		/// Para hacer actualización del estado del juego.
		/// 
		/// Es preferible usar ECS para manejar actualizaciones
		/// del estado del juego.
		/// </summary>
		/// <param name="deltaTime">Tiempo que ha pasado desde el último frame,
		/// en segundos.</param>
		virtual void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Función que se ejecuta al salir del juego.
		/// </summary>
		virtual void OnExit();


		/// <summary>
		/// Inicia la ejecución del juego.
		/// </summary>
		void Run();

		/// <summary>
		/// Fuerza el shutdown del juego.
		/// </summary>
		void Exit();

		/// <summary>
		/// Devuelve el número de frames en el último segundo.
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
