#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "WindowAPI.h"
#include "VulkanRenderer.h"
#include "AudioAPI.h"
#include "ECS.h"
#include "PhysicsSystem.h"
#include "InputSystem.h"
#include "RenderSystem3D.h"
#include "OnTickSystem.h"
#include "Scene.h"

#include "UniquePtr.hpp"

/// <summary>
/// Esta clase repersenta la clase principal de un juego en OSKengine.
/// Aunque no es necesario usarla para usar OSKengine,
/// provee muchos elementos que hacen más sencillo su desarrollo, incluyendo: <br/>
/// Inicialización de renderizador, ventana y sistema de audio.  <br/>
/// Sistema ECS. <br/>
/// Sistema de Renderizado 3D.
/// </summary>
class Game {

public:

	virtual ~Game() = default;

	/// <summary>
	/// Crea la ventana a partir de los ajustes de WindowCreateInfo.
	/// </summary>
	void SetupWindow();

	/// <summary>
	/// Crea el renderizador a partir de los ajustes de RendererCreateInfo.
	/// </summary>
	void SetupRenderer();


	/// <summary>
	/// Primera función que se llama al crear el jeugo.
	/// Úsese para esablecer los ajustes del renderizador y de la ventana.
	/// </summary>
	virtual void OnCreate() {}

	/// <summary>
	/// Úsese para cargar los elementos globales.
	/// </summary>
	virtual void LoadContent() {}

	/// <summary>
	/// Función que se llama al cerrar el juego.
	/// </summary>
	virtual void OnExit() {}

	/// <summary>
	/// Función que se llama una vez por tick.
	/// </summary>
	/// <param name="deltaTime">Delta.</param>
	virtual void OnTick(deltaTime_t deltaTime) {}

	/// <summary>
	/// Úsese para actualizar los spritebatches.
	/// Se llamad espués del renderizado 3D.
	/// </summary>
	virtual void OnDraw2D() {}

	/// <summary>
	/// Ejecuta el juego.
	/// </summary>
	void Run();

	/// <summary>
	/// Cierra el juego.
	/// </summary>
	void Exit();

	/// <summary>
	/// Devuelve los FPS actuales.
	/// </summary>
	/// <returns>FPS.</returns>
	uint32_t GetFPS() const;

	/// <summary>
	/// Devuelve el renderizador.
	/// </summary>
	/// <returns>Renderizador.</returns>
	OSK::RenderAPI* GetRenderer();

	/// <summary>
	/// Devuelve la ventana.
	/// </summary>
	/// <returns>Ventana.</returns>
	OSK::WindowAPI* GetWindow();

	/// <summary>
	/// Devuelve el sistema de audio.
	/// </summary>
	/// <returns>Sistema de audio.</returns>
	OSK::AudioSystem* GetAudioSystem();


	/// <summary>
	/// Información de la ventana que se va a crear.
	/// </summary>
	struct WindowCreateInfo_t {
		uint32_t SizeX = 1280;
		uint32_t SizeY = 720;
		std::string Name = "OSKengine";

		OSK::MouseAccelerationMode MouseAcceleration = OSK::MouseAccelerationMode::RAW;
		OSK::MouseInputMode MouseMode = OSK::MouseInputMode::ALWAYS_RETURN;
	} windowCreateInfo;

	/// <summary>
	/// Ajustes del renderizador.
	/// </summary>
	struct RendererCreateInfo_t {
		OSK::PresentMode VSyncMode = OSK::PresentMode::VSYNC;
		float FPSlimit = std::numeric_limits<float>::max();

		std::string GameName = "OSKengine Project";
		OSK::Version GameVersion = { 0,0,0 };

		float RendererResolution = 1.0f;
	} rendererCreateInfo;

	/// <summary>
	/// Content manager global.
	/// </summary>
	OSK::ContentManager* content = nullptr;

private:

	/// <summary>
	/// Inicia el juego.
	/// </summary>
	void Init();

	/// <summary>
	/// Inicia los sistemas ECS.
	/// </summary>
	void SetupSystems();

	/// <summary>
	/// Cierra el juego.
	/// </summary>
	void Close();

	/// <summary>
	/// Bucle infinito principal.
	/// </summary>
	void MainLoop();

	/// <summary>
	/// Renderizador.
	/// </summary>
	UniquePtr<OSK::RenderAPI> renderer;

	/// <summary>
	/// Ventana.
	/// </summary>
	UniquePtr<OSK::WindowAPI> window;

	/// <summary>
	/// Sistema de audio.
	/// </summary>
	UniquePtr<OSK::AudioSystem> audio;

	/// <summary>
	/// FPS.
	/// </summary>
	uint32_t framerate = 0;

	/// <summary>
	/// Contador de un segundo, para actualizar el valor de FPS.
	/// </summary>
	deltaTime_t framerateDeltaTime = 0.0f;

	/// <summary>
	/// Número de frames desde la última vez que se reseteó el FramerateDeltaTime.
	/// </summary>
	uint32_t framerateCount = 0;

protected:

	/// <summary>
	/// Sistema ECS.
	/// </summary>
	UniquePtr<OSK::EntityComponentSystem> entityComponentSystem;

	/// <summary>
	/// Sistema ECS de físicas.
	/// </summary>
	UniquePtr<OSK::PhysicsSystem> physicsSystem = nullptr;

	/// <summary>
	/// Sistema ECS de input.
	/// </summary>
	UniquePtr<OSK::InputSystem> inputSystem = nullptr;

	/// <summary>
	/// Sistema ECS de renderizado.
	/// </summary>
	UniquePtr<OSK::RenderSystem3D> renderSystem3D = nullptr;

	/// <summary>
	/// Sistema ECS de OnTick.
	/// </summary>
	UniquePtr<OSK::OnTickSystem> onTickSystem = nullptr;

	/// <summary>
	/// Estado del teclado en el anterior frame.
	/// </summary>
	OSK::KeyboardState oldKeyboardState{};

	/// <summary>
	/// Estado del teclado en el actual frame.
	/// </summary>
	OSK::KeyboardState newKeyboardState{};

	/// <summary>
	/// Estado del ratón en el anterior frame.
	/// </summary>
	OSK::MouseState oldMouseState{};

	/// <summary>
	/// Estado del ratón en el actual frame.
	/// </summary>
	OSK::MouseState newMouseState{};

	/// <summary>
	/// Spritebatch global.
	/// </summary>
	OSK::SpriteBatch spriteBatch;

	/// <summary>
	/// Escena actual.
	/// </summary>
	UniquePtr<OSK::Scene> scene;

};
