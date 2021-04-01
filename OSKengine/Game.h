#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "WindowAPI.h"
#include "VulkanRenderer.h"
#include "AudioAPI.h"
#include "ECS.h"
#include "PhysicsScene.h"
#include "InputSystem.h"
#include "RenderSystem3D.h"
#include "OnTickSystem.h"

#include "UniquePtr.hpp"

class Game {

public:

	virtual ~Game() = default;

	void SetupWindow();
	void SetupRenderer();

	virtual void OnCreate() {}
	virtual void LoadContent() {}
	virtual void OnExit() {}
	virtual void OnTick(deltaTime_t deltaTime) {}
	virtual void OnDraw2D() {}

	void Run();
	void Exit();

	float GetFPS();

	OSK::RenderAPI* GetRenderer();
	OSK::WindowAPI* GetWindow();
	OSK::AudioSystem* GetAudioSystem();

	struct WindowCreateInfo_t {
		uint32_t SizeX = 1280;
		uint32_t SizeY = 720;
		std::string Name = "OSKengine";

		OSK::MouseMovementMode MouseAcceleration = OSK::MouseMovementMode::RAW;
		OSK::MouseInputMode MouseMode = OSK::MouseInputMode::ALWAYS_RETURN;
	} WindowCreateInfo;

	struct RendererCreateInfo_t {
		OSK::PresentMode VSyncMode = OSK::PresentMode::VSYNC;
		float FPSlimit = std::numeric_limits<float>::max();

		std::string GameName = "OSKengine Project";
		OSK::Version GameVersion = { 0,0,0 };

		float RendererResolution = 1.0f;
	} RendererCreateInfo;

	OSK::ContentManager* Content = nullptr;

private:

	void Init();
	void SetupSystems();

	void Close();

	void MainLoop();

	UniquePtr<OSK::RenderAPI> renderer;
	UniquePtr<OSK::WindowAPI> window;
	UniquePtr<OSK::AudioSystem> audio;

	deltaTime_t Framerate = 0.0f;
	deltaTime_t FramerateDeltaTime = 0.0f;
	uint32_t FramerateCount = 0;

protected:

	UniquePtr<OSK::EntityComponentSystem> ECS;

	OSK::PhysicsScene* PhysicsSystem = nullptr;
	OSK::InputSystem* InputSystem = nullptr;
	OSK::RenderSystem3D* RenderSystem3D = nullptr;
	OSK::OnTickSystem* OnTickSystem = nullptr;

	OSK::KeyboardState OldKS{};
	OSK::KeyboardState NewKS{};

	OSK::MouseState OldMS{};
	OSK::MouseState NewMS{};

	OSK::SpriteBatch SpriteBatch;

};


