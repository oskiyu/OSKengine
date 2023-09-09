#pragma once

#include <OSKengine/Game.h>

#include <OSKengine/GameObject.h>
#include <OSKengine/FrameCombiner.h>
#include <OSKengine/RenderTarget.h>

#include <OSKengine/BloomPass.h>
#include <OSKengine/ToneMapping.h>

#include "CarSpawner.h"
#include "CircuitSpawner.h"

#include "CollisionTests.h"

class Game : public OSK::IGame {

public:

	Game() noexcept = default;

private:

	void CreateWindow() override;
	void SetupEngine() override;

	void OnCreate() override;
	void OnTick(TDeltaTime deltaTime) override;
	void OnExit() override;

	void OnWindowResize(const OSK::Vector2ui& newRes) override;

	void BuildFrame() override;

	void RegisterEcse();
	void SetupUi();

	void ToMainMenu();

	void PauseSystems();
	void UnpauseSystems();

private:

	OSK::ECS::GameObjectIndex firstCar = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex secondCar = OSK::ECS::EMPTY_GAME_OBJECT;

	CollisionTesting collisionTesting{};

	CarSpawner carSpawner{};
	CircuitSpawner circuitSpawner{};

	OSK::ECS::GameObjectIndex cameraObject	 = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraObject2d = OSK::ECS::EMPTY_GAME_OBJECT;
	
	OSK::UniquePtr<OSK::GRAPHICS::FrameCombiner> finalFrameCombiner;
	OSK::UniquePtr<OSK::GRAPHICS::RenderTarget> textRenderTarget;

	OSK::UniquePtr<OSK::GRAPHICS::BloomPass> bloomPass;
	OSK::UniquePtr<OSK::GRAPHICS::ToneMappingPass> toneMappingPass;

	OSK::GRAPHICS::Material* material2d = nullptr;
	OSK::GRAPHICS::Material* material3d = nullptr;

};
