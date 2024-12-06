#pragma once

// Para OSKAPI_CALL.
#include "ApiCall.h"

// Tipos.
#include "NumericTypes.h"
#include "Vector2.hpp"

// Interfaz.
#include "IUiContainer.h"
#include "UiFreeContainer.h" // Para el root.

#include "UniquePtr.hpp"

#include "DefaultElementsProfiles.h"

namespace OSK {

	struct Version;

	/// @brief Esta clase representa la clase principal de un juego de OSKengine.
	/// No es estrictamente necesaria para desarrollar con OSKengine, pero
	/// es un buen punto de partida.
	/// Ofrece una abstracción mínima.
	/// 
	/// @pre Deben sobreescribirse las funciones de CreateWindow()
	/// y SetupEngine() de manera obligatoria.
	class OSKAPI_CALL IGame {

	public:

		virtual ~IGame() = default;

#pragma region ECS
		
		/// @brief Registra los assets y los loaders específicos del 
		/// juego.
		virtual void RegisterAssets();

		/// @brief Registra los componentes específicos del juego.
		virtual void RegisterComponents();

		/// @brief Registra los sistemas específicos del juego.
		virtual void RegisterSystems();

		/// @brief Registra los comandos de consola.
		virtual void RegisterConsoleCommands();

#pragma endregion

		
		/// @brief Función ejecutada al cargar el juego.
		/// @note Se ejecuta después de haber cargado todos
		/// los sistemas del juego.
		virtual void OnCreate();

		/// @brief Función que se ejecuta cada frame, antes que los
		/// sistemas ECS.
		/// @param deltaTime Tiempo que ha pasado desde la última ejecución,
		/// en segundos.
		virtual void OnTick_BeforeEcs(TDeltaTime deltaTime);

		/// @brief Función que se ejecuta cada frame, después que los
		/// sistemas ECS.
		/// @param deltaTime Tiempo que ha pasado desde la última ejecución,
		/// en segundos.
		virtual void OnTick_AfterEcs(TDeltaTime deltaTime);

		/// @brief Esta función debe renderizar la imagen final que se representará en la ventana.
		/// 
		/// Los comandos de renderizado finales deben ejecutarse en el renderpass principal
		/// del motor (IRenderer::GetMainRenderpass).
		virtual void BuildFrame() = 0;

		/// @brief Función que se ejecuta al salir del juego.
		virtual void OnExit();

		/// @brief Función que se ejecuta cuando la ventana cambia de tamaño.
		/// @param size Nueva resolución, en píxeles.
		virtual void OnWindowResize(const Vector2ui& size);


		/// @brief Inicia la ejecución del juego.
		void _Run();

		/// @brief Fuerza el shutdown del juego.
		void Exit();


		/// @return Devuelve el número de frames en el último segundo.
		USize32 GetFps() const;

		/// @return Elemento raíz de la interfaz de usuario.
		UI::IContainer& GetRootUiElement();
		/// @return Elemento raíz de la interfaz de usuario.
		const UI::IContainer& GetRootUiElement() const;

	protected:

		explicit IGame(GAME::DefaultContentProfile defaultContentProfile);

		/// @brief Debe crear la ventana del Engine.
		virtual void CreateWindow() = 0;

		/// @brief Debe inicializar el renderizador del Engine.
		virtual void SetupEngine() = 0;

	private:

		void HandleResizeEvents();
		void UpdateFps(TDeltaTime deltaTime);

		GAME::DefaultContentProfile m_defaultContentProfile = GAME::DefaultContentProfile::ALL;

		TDeltaTime m_deltaTime = 1.0f;

		TDeltaTime m_framerateCountTimer = 0.0f;
		USize32 m_currentFps = 0;
		USize32 m_frameCount = 0;

		UniquePtr<UI::IContainer> m_rootUiElement;

	};

}
