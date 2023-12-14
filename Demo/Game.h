#pragma once

#include <OSKengine/Game.h>

#include <OSKengine/GameObject.h>
#include <OSKengine/FrameCombiner.h>
#include <OSKengine/RenderTarget.h>

#include <OSKengine/BloomPass.h>
#include <OSKengine/HbaoPass.h>
#include <OSKengine/ToneMapping.h>

#include <OSKengine/UiImageView.h>
#include <OSKengine/UiTextView.h>

#include "CarSpawner.h"
#include "CircuitSpawner.h"
#include "SelectionHub.h"

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

	void SetupPostProcessingChain();

	void RegisterAssets();
	void RegisterEcse();

	void SetupUi();

	void SetMainCar(OSK::ECS::GameObjectIndex obj);

	void CheckEvents();

	void ToMainMenu();
	void ToGame(std::string_view carAssetsPath);

	void PauseSystems();
	void UnpauseSystems();

	void SpanwCamera();
	void SetupTreeNormals();

private:

	bool m_startNextFrame = false;

	OSK::UI::TextView* gearText = nullptr;
	OSK::UI::TextView* rpmText = nullptr;
	OSK::UI::TextView* speedText = nullptr;

	OSK::UI::ImageView* chasGtSelectedCar = nullptr;
	OSK::UI::ImageView* owSelectedCar = nullptr;
	OSK::UI::ImageView* lwmSelectedCar = nullptr;
	OSK::UI::ImageView* currentlySelectedCar = nullptr;


	OSK::ECS::GameObjectIndex firstCar = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex circuit = OSK::ECS::EMPTY_GAME_OBJECT;

	CollisionTesting collisionTesting{};

	CarSpawner carSpawner{};
	CircuitSpawner circuitSpawner{};
	SelectionHub m_hub;

	OSK::ECS::GameObjectIndex cameraObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraArmObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraObject2d = OSK::ECS::EMPTY_GAME_OBJECT;
	
	OSK::UniquePtr<OSK::GRAPHICS::FrameCombiner> finalFrameCombiner;
	OSK::UniquePtr<OSK::GRAPHICS::RenderTarget> textRenderTarget;

	OSK::UniquePtr<OSK::GRAPHICS::BloomPass> bloomPass;
	OSK::UniquePtr<OSK::GRAPHICS::HbaoPass> hbaoPass;
	OSK::UniquePtr<OSK::GRAPHICS::ToneMappingPass> toneMappingPass;

	OSK::GRAPHICS::Material* material2d = nullptr;
	OSK::GRAPHICS::Material* material3d = nullptr;

};
