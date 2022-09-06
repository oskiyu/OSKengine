#pragma once

#include "OSKmacros.h"
#include "Vector2.hpp"

#include <string>

namespace OSK {

	struct Version;

	/// <summary>
	/// Esta clase representa la clase principal de un juego de OSKengine.
	/// No es estrictamente necesaria para desarrollar con OSKengine, pero
	/// es un buen punto de partida.
	/// Ofrece una abstracci�n m�nima.
	/// 
	/// @pre Deben sobreescribirse las funciones de CreateWindow()
	/// y SetupEngine() de manera obligatoria.
	/// </summary>
	class OSKAPI_CALL IGame {

	public:

		virtual ~IGame() = default;

		/// <summary>
		/// Registra los assets y los loaders espec�ficos del 
		/// juego.
		/// </summary>
		virtual void RegisterAssets();

		/// <summary>
		/// Registra los componentes espec�ficos del juego.
		/// </summary>
		virtual void RegisterComponents();

		/// <summary>
		/// Registra los sistemas espec�ficos del juego.
		/// </summary>
		virtual void RegisterSystems();

		/// <summary>
		/// Funci�n ejecutada al cargar el juego.
		/// 
		/// @note Se ejecuta dfespu�s de haber cargado todos
		/// los sistemas del juego.
		/// </summary>
		virtual void OnCreate();

		/// <summary>
		/// Funci�n que se ejecuta cada frame.
		/// Para hacer actualizaci�n del estado del juego.
		/// 
		/// @note Es preferible usar ECS para manejar actualizaciones
		/// del estado del juego.
		/// </summary>
		/// <param name="deltaTime">Tiempo que ha pasado desde el �ltimo frame,
		/// en segundos.</param>
		virtual void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Esta funci�n debe renderizar la imagen final que se representar� en la ventana.
		/// 
		/// Los comandos de renderizado finales deben ejecutarse en el renderpass principal
		/// del motor (IRenderer::GetMainRenderpass).
		/// 
		/// Para construir el frame, se pueden usar las im�genes de los sistemas de renderizado.
		/// </summary>
		virtual void BuildFrame() = 0;

		/// <summary>
		/// Funci�n que se ejecuta al salir del juego.
		/// </summary>
		virtual void OnExit();

		/// <summary>
		/// Funci�n que se ejecuta cuando la ventana cambia de tama�o.
		/// </summary>
		virtual void OnWindowResize(const Vector2ui& size);

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
