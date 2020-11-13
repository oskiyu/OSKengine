#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

#ifndef OSK_RELEASE_DLL

/**/
#include "VulkanRenderer.h"
#include "Camera2D.h"
#include "Sprite.h"
#include "Panel.h"
#include "Button.h"
#include "ToString.h"
#include "Log.h"

#include "PhysicsScene.h"

#include "Profiler.h"

#include "SAT_Collider.h"
#include "RayCast.h"

/**/
class Entity {

public:

	Entity() {
		setup();
	}

	Entity(OSK::Vector3f position) {
		Transform.SetPosition(position);
		setup();
	}

	OSK::PhysicsEntity Physics;
	OSK::Model model;
	OSK::Transform Transform;

private:
	void setup() {
		Physics.EntityTransform = &Transform;
		Physics.Collision.ColliderTransform.AttachTo(&Transform);
		model.ModelTransform = &Transform;

		auto box = OSK::Collision::SAT_Collider::CreateOBB();
		Physics.Collision.SatColliders.push_back(box);
		Physics.Collision.SatColliders.back().BoxTransform.AttachTo(&Transform);
		Physics.Collision.BroadType = OSK::BroadColliderType::BOX_AABB;
		Physics.Collision.BroadCollider.Box.Size = { 5.0f };
	}
};
/**/

int program() {

	bool showStartup = false;
#ifndef OSK_DEBUG
	showStartup = true;
#endif

	float OSKshow = 1.25f;
	float OSKengine_show = 1.25f;
	//

	OSK::Profiler Profiler{};

	OSK::WindowAPI* windowAPI = new OSK::WindowAPI();
	windowAPI->SetWindow(1280, 720, "OSKengine Vk", OSK::GraphicsAPI::VULKAN);
	windowAPI->SetMouseMovementMode(OSK::MouseMovementMode::RAW);
	windowAPI->SetMouseMode(OSK::MouseInputMode::ALWAYS_RETURN);

	OSK::RenderAPI RenderAPI{};
	RenderAPI.Window = windowAPI;
	RenderAPI.SetPresentMode(OSK::PresentMode::VSYNC);
	RenderAPI.FPSlimit = INFINITE;
	OskResult result = RenderAPI.Init(OSK::RenderMode::RENDER_MODE_2D_AND_3D, "OSKengine", OSK::Version{ 0, 0, 0 });
	if (result != OskResult::SUCCESS) {
		OSK_SHOW_TRACE();
		return (int)result;
	}

	OSK::Sprite texture{};
	RenderAPI.Content->LoadSprite(texture, "models/cube/td.png");

	OSK::Font* fuente = RenderAPI.Content->LoadFont("Fonts/AGENCYB.ttf", 20);

	OSK::Font* showFont = nullptr;
	showFont = RenderAPI.Content->LoadFont("Fonts/AGENCYB.ttf", 40);

	OSK::RenderizableScene* Scene = new OSK::RenderizableScene(&RenderAPI, 32);

	if (!showStartup)
		RenderAPI.SetRenderizableScene(Scene);

	//
	OSK::Model model{};
	Scene->LoadSkybox("skybox/skybox.ktx");
	Scene->LoadHeightmap("heightmaps/Heightmap.png", { 5.0f }, 25);
	RenderAPI.Content->LoadModel(model, "models/cube/cube.obj");
	model.ModelTransform = new OSK::Transform();
	model.ModelTransform->SetScale(0.5f);
	//Scene->AddModel(&model);
	//Scene->terreno->FrictionCoefficient = 0;

	OSK::ProfilingUnit mainDrawUnit{ "Main Draw()" };
	OSK::ProfilingUnit drawStringUnit{ "StringMemory" };
	OSK::ProfilingUnit mainUpdateUnit{ "Main Update()" };
	Profiler.AddProfilingUnit(&RenderAPI.renderP_Unit);
	Profiler.AddProfilingUnit(&RenderAPI.updateCmdP_Unit);
	Profiler.AddProfilingUnit(&mainDrawUnit);
	Profiler.AddProfilingUnit(&mainUpdateUnit);
	Profiler.AddProfilingUnit(&drawStringUnit);

	OSK::SpriteBatch spriteBatch = RenderAPI.CreateSpriteBatch();

#pragma region Physics testing.

	Entity EntityA(OSK::Vector3f{ 0.0f , -20.0f, 0.0f });
	RenderAPI.Content->LoadModel(EntityA.model, "models/cube/cube.obj");
	//EntityA.Physics.Type = OSK::PhysicalEntityMobilityType::STATIC;
	Entity EntityB(OSK::Vector3f{ 0.0f, -25.0f, 0.0f });
	RenderAPI.Content->LoadModel(EntityB.model, "models/cube/cube.obj");
	EntityB.Transform.RotateWorldSpace(20.0f, { 1, 0, 0 });

	OSK::PhysicsScene PhysicsScene;
	PhysicsScene.AddEntity(&EntityA.Physics);
	PhysicsScene.AddEntity(&EntityB.Physics);
	PhysicsScene.FloorTerrain = Scene->Terreno;

	Scene->AddModel(&EntityA.model);
	Scene->AddModel(&EntityB.model);

#pragma endregion

	RenderAPI.OSKengineIconSprite.SpriteTransform.SetPosition({ 5.0f });
	RenderAPI.OSKengineIconSprite.SpriteTransform.SetScale({ 48.f });

	if (showStartup) {
		RenderAPI.OSK_IconSprite.SpriteTransform.SetPosition(windowAPI->GetRectangle().GetRectangleMiddlePoint() - OSK::Vector2f(256.0f) / 2);
		RenderAPI.OSK_IconSprite.SpriteTransform.SetScale({ 256.0f });
		RenderAPI.OSKengineIconSprite.SpriteTransform.SetPosition(windowAPI->GetRectangle().GetRectangleMiddlePoint() - OSK::Vector2f(256.0f) / 2);
		RenderAPI.OSKengineIconSprite.SpriteTransform.SetScale({ 256.0f });
	}

	RenderAPI.SubmitSpriteBatch(spriteBatch);
	RenderAPI.DefaultCamera2D.UseTargetSize = false;
	texture.SpriteTransform.SetPosition(windowAPI->GetRectangle().GetRectangleMiddlePoint());
	texture.SpriteTransform.SetScale({ -5, 5 });
	texture.SetTexCoords(0, 0, 100, 100);

	OSK::KeyboardState OldKS = {};
	OSK::KeyboardState NewKS = {};
	OSK::MouseState OldMS = {};
	OSK::MouseState NewMS = {};

	windowAPI->UpdateKeyboardState(OldKS);
	windowAPI->UpdateKeyboardState(NewKS);
	windowAPI->UpdateMouseState(OldMS);
	windowAPI->UpdateMouseState(NewMS);
	double deltaTime = 0.016;

	OSK::ReservedText CopyrightText;
	CopyrightText.SetText("OSKengine " + std::string(OSK::ENGINE_VERSION));
	spriteBatch.PrecalculateText(showFont, CopyrightText, 0.5f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);

	OSK::ReservedText PosText;

	float FPS = 0.0f;
	float totalDeltaTime = 0.0f;
	int count = 0;
	while (!windowAPI->WindowShouldClose()) {
		mainUpdateUnit.Start();

		double startTime = windowAPI->GetTime();
		windowAPI->PollEvents();
		windowAPI->UpdateKeyboardState(NewKS);
		windowAPI->UpdateMouseState(NewMS);

		if (showStartup) {
			if (OSKshow > 0.0f) {
				OSKshow -= deltaTime;
				RenderAPI.OSK_IconSprite.SpriteTransform.SetPosition(windowAPI->GetRectangle().GetRectangleMiddlePoint() - OSK::Vector2f(256.0f) / 2);
				RenderAPI.OSK_IconSprite.SpriteTransform.SetScale({ 256.0f });
			}
			else if (OSKengine_show > 0.0f) {
				OSKengine_show -= deltaTime;
				RenderAPI.OSKengineIconSprite.SpriteTransform.SetPosition(windowAPI->GetRectangle().GetRectangleMiddlePoint() - OSK::Vector2f(256.0f) / 2);
				RenderAPI.OSKengineIconSprite.SpriteTransform.SetScale({ 256.0f });
			}
			else {
				RenderAPI.OSKengineIconSprite.SpriteTransform.SetPosition({ 5.0f });
				RenderAPI.OSKengineIconSprite.SpriteTransform.SetScale({ 48.0f });

				RenderAPI.SetRenderizableScene(Scene);

				showStartup = false;
			}
		}

		if (NewKS.IsKeyDown(OSK::Key::F11) && OldKS.IsKeyUp(OSK::Key::F11)) {
			windowAPI->SetFullscreen(!windowAPI->IsFullscreen);
			texture.SpriteTransform.SetPosition(windowAPI->GetRectangle().GetRectangleMiddlePoint());
			RenderAPI.DefaultCamera2D.TargetSize = windowAPI->GetRectangle().GetRectangleSize();
			spriteBatch.PrecalculateText(showFont, CopyrightText, 0.5f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);
		}

		const float SPEED = 3.0f;

		if (NewKS.IsKeyDown(OSK::Key::ESCAPE))
			windowAPI->Close();

		if (NewKS.IsKeyDown(OSK::Key::UP))
			EntityA.Physics.EntityTransform->AddPosition(OSK::Vector3f(3, 0, 0) * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::DOWN))
			EntityA.Physics.EntityTransform->AddPosition(OSK::Vector3f(-3, 0, 0) * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::LEFT))
			EntityA.Physics.EntityTransform->AddPosition(OSK::Vector3f(0, 0, -3) * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::RIGHT))
			EntityA.Physics.EntityTransform->AddPosition(OSK::Vector3f(0, 0, 3) * deltaTime);

		if (NewKS.IsKeyDown(OSK::Key::Z)) {
			EntityA.Transform.RotateWorldSpace(deltaTime * 2, { 0, 1, 0 });
		}
		if (NewKS.IsKeyDown(OSK::Key::X)) {
			EntityA.Transform.RotateWorldSpace(deltaTime  * 2, { 1, 0, 0 });
		}
		
		if (NewKS.IsKeyDown(OSK::Key::W))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(RenderAPI.DefaultCamera3D.Front * SPEED * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::S))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(-RenderAPI.DefaultCamera3D.Front * SPEED * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::A))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(-RenderAPI.DefaultCamera3D.Right * SPEED * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::D))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(RenderAPI.DefaultCamera3D.Right * SPEED * deltaTime);

		if (NewKS.IsKeyDown(OSK::Key::P) && OldKS.IsKeyUp(OSK::Key::P)) {
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, "FPS: " + std::to_string(FPS));
			Profiler.ShowData();
			EntityA.Transform.SetPosition(OSK::Vector3f{ 5.0f, 0.0f + Scene->Terreno->GetHeight({5.0f}), 5.0f });
			EntityB.Transform.SetPosition(OSK::Vector3f{ 5.0f, -5.0f + Scene->Terreno->GetHeight({5.0f}), 5.0f });

			OSK::Logger::DebugLog(ToString(EntityA.Transform.GlobalPosition));
			OSK::Logger::DebugLog(ToString(EntityB.Transform.GlobalPosition));
		}

		PhysicsScene.Simulate(deltaTime, 1.0f);

		mouseVar_t deltaX = NewMS.PositionX - OldMS.PositionX;
		mouseVar_t deltaY = NewMS.PositionY - OldMS.PositionY;
		RenderAPI.DefaultCamera3D.Girar(deltaX, -deltaY);

		if (NewKS.IsKeyDown(OSK::Key::V) && OldKS.IsKeyUp(OSK::Key::V))
			RenderAPI.SetPresentMode(OSK::PresentMode::VSYNC);
		if (NewKS.IsKeyDown(OSK::Key::B) && OldKS.IsKeyUp(OSK::Key::B))
			RenderAPI.SetPresentMode(OSK::PresentMode::VSYNC_TRIPLE_BUFFER);

		auto info = EntityA.Physics.Collision.SatColliders[0].GetCollisionInfo(EntityB.Physics.Collision.SatColliders[0]);
		model.ModelTransform->SetPosition(EntityA.Transform.GlobalPosition + info.PointA);

		totalDeltaTime += deltaTime;
		count++;
		if (totalDeltaTime > 1.0f) {
			FPS = count;
			count = 0;

			totalDeltaTime = 0.0f;
		}
		mainUpdateUnit.End();

		//Draw();
		{
			mainDrawUnit.Start();
			RenderAPI.DefaultCamera3D.updateVectors();
			Scene->Lights.Points[0].Position = RenderAPI.DefaultCamera3D.CameraTransform.GlobalPosition.ToVector3f().ToGLM();

			spriteBatch.Clear();
			if (showStartup) {
				if (OSKshow > 0.0f) {
					spriteBatch.DrawSprite(RenderAPI.OSK_IconSprite);
				}
				else if (OSKengine_show > 0.0f) {
					spriteBatch.DrawString(showFont, "Powered by:", 1.0f, OSK::Vector2(0, -150), OSK::Color(0.02f), OSK::Anchor::CENTER);
					spriteBatch.DrawSprite(RenderAPI.OSKengineIconSprite);
				}
			}
			else {

				spriteBatch.DrawSprite(texture);
				spriteBatch.DrawSprite(RenderAPI.OSKengineIconSprite);

				spriteBatch.DrawString(showFont, "FPS: " + std::to_string((int)FPS), 1, OSK::Vector2(0, 5), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);

				spriteBatch.DrawString(fuente, "Resolution mult: " + std::to_string(RenderAPI.RenderResolutionMultiplier), 1, OSK::Vector2(-100, 25), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
				spriteBatch.DrawString(fuente, "Renderer res: " + OSK::ToString(OSK::Vector2ui(RenderAPI.RenderTargetSizeX, RenderAPI.RenderTargetSizeY)), 1, OSK::Vector2(-100, 45), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
				spriteBatch.DrawString(fuente, "Output res: " + OSK::ToString((OSK::Vector2f(RenderAPI.RenderTargetSizeX, RenderAPI.RenderTargetSizeY) / RenderAPI.RenderResolutionMultiplier).ToVector2ui()), 1, OSK::Vector2(-100, 65), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);

				spriteBatch.DrawString(CopyrightText);

				auto info = OSK::RayCast::CastRay(RenderAPI.DefaultCamera3D.CameraTransform.GlobalPosition.ToVector3f(), RenderAPI.DefaultCamera3D.Front.ToVector3f(), EntityA.Physics.Collision.SatColliders[0]);

				PosText.SetText(OSK::ToString(RenderAPI.DefaultCamera3D.CameraTransform.GlobalPosition));
				//spriteBatch.DrawString(showFont, PosText, 0.5f, OSK::Vector2(0, 25), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
			}

			RenderAPI.SubmitSpriteBatch(spriteBatch);
			mainDrawUnit.End();
		}

		RenderAPI.RenderFrame();

		OldKS = NewKS;
		OldMS = NewMS;
		double endTime = windowAPI->GetTime();
		deltaTime = endTime - startTime;
	}

	delete Scene;

	RenderAPI.Close();

	delete windowAPI;

	return 0;
}

int main() {

	OSK::Logger::Start();

	try {
		program();
	}
	catch (std::runtime_error& e) {
		OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, e.what());
	}

	OSK::Logger::Close();

	return 0;
}

#endif