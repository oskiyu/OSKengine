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

#include "UniquePtr.hpp"

/// <summary>
/// Esta clase repersenta la clase principal de un juego en OSKengine.
/// Aunque no es necesario usarla para usar OSKengine,
/// provee muchos elementos que hacen m�s sencillo su desarrollo, incluyendo: <br/>
/// Inicializaci�n de renderizador, ventana y sistema de audio.  <br/>
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
	/// Primera funci�n que se llama al crear el jeugo.
	/// �sese para esablecer los ajustes del renderizador y de la ventana.
	/// </summary>
	virtual void OnCreate() {}

	/// <summary>
	/// �sese para cargar los elementos globales.
	/// </summary>
	virtual void LoadContent() {}

	/// <summary>
	/// Funci�n que se llama al cerrar el juego.
	/// </summary>
	virtual void OnExit() {}

	/// <summary>
	/// Funci�n que se llama una vez por tick.
	/// </summary>
	/// <param name="deltaTime">Delta.</param>
	virtual void OnTick(deltaTime_t deltaTime) {}

	/// <summary>
	/// �sese para actualizar los spritebatches.
	/// Se llamad espu�s del renderizado 3D.
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
	float GetFPS();

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
	/// Informaci�n de la ventana que se va a crear.
	/// </summary>
	struct WindowCreateInfo_t {
		uint32_t SizeX = 1280;
		uint32_t SizeY = 720;
		std::string Name = "OSKengine";

		OSK::MouseAccelerationMode MouseAcceleration = OSK::MouseAccelerationMode::RAW;
		OSK::MouseInputMode MouseMode = OSK::MouseInputMode::ALWAYS_RETURN;
	} WindowCreateInfo;

	/// <summary>
	/// Ajustes del renderizador.
	/// </summary>
	struct RendererCreateInfo_t {
		OSK::PresentMode VSyncMode = OSK::PresentMode::VSYNC;
		float FPSlimit = std::numeric_limits<float>::max();

		std::string GameName = "OSKengine Project";
		OSK::Version GameVersion = { 0,0,0 };

		float RendererResolution = 1.0f;
	} RendererCreateInfo;

	/// <summary>
	/// Content manager global.
	/// </summary>
	OSK::ContentManager* Content = nullptr;

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
	deltaTime_t Framerate = 0.0f;

	/// <summary>
	/// Contador de un segundo, para actualizar el valor de FPS.
	/// </summary>
	deltaTime_t FramerateDeltaTime = 0.0f;

	/// <summary>
	/// N�mero de frames desde la �ltima vez que se resete� el FramerateDeltaTime.
	/// </summary>
	uint32_t FramerateCount = 0;

protected:

	/// <summary>
	/// Sistema ECS.
	/// </summary>
	UniquePtr<OSK::EntityComponentSystem> ECS;

	/// <summary>
	/// Sistema ECS de f�sicas.
	/// </summary>
	OSK::PhysicsSystem* PhysicsSystem = nullptr;

	/// <summary>
	/// Sistema ECS de input.
	/// </summary>
	OSK::InputSystem* InputSystem = nullptr;

	/// <summary>
	/// Sistema ECS de renderizado.
	/// </summary>
	OSK::RenderSystem3D* RenderSystem3D = nullptr;

	/// <summary>
	/// Sistema ECS de OnTick.
	/// </summary>
	OSK::OnTickSystem* OnTickSystem = nullptr;

	/// <summary>
	/// Estado del teclado en el anterior frame.
	/// </summary>
	OSK::KeyboardState OldKS{};

	/// <summary>
	/// Estado del teclado en el actual frame.
	/// </summary>
	OSK::KeyboardState NewKS{};

	/// <summary>
	/// Estado del rat�n en el anterior frame.
	/// </summary>
	OSK::MouseState OldMS{};

	/// <summary>
	/// Estado del rat�n en el actual frame.
	/// </summary>
	OSK::MouseState NewMS{};

	/// <summary>
	/// Spritebatch global.
	/// </summary>
	OSK::SpriteBatch SpriteBatch;

};