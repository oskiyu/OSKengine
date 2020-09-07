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

template <typename T> struct Vector3_t {
	T x;
	T y;
	T z;
};

int program() {

	OSK::WindowAPI* windowAPI = new OSK::WindowAPI();
	windowAPI->SetWindow(1280, 720, "OSKengine Vk", OSK::GraphicsAPI::VULKAN);
	windowAPI->SetMouseMovementMode(OSK::MouseMovementMode::RAW);
	windowAPI->SetMouseMode(OSK::MouseInputMode::ALWAYS_RETURN);

	OSK::VulkanRenderer RenderAPI{};
	RenderAPI.Window = windowAPI;
	RenderAPI.Settings.MaxTextures = 1024;
	RenderAPI.SetPresentMode(OSK::PresentMode::VSYNC);
	RenderAPI.FPSlimit = INFINITE;
	OskResult result = RenderAPI.Init(OSK::RenderMode::RENDER_MODE_2D_AND_3D, "XD", OSK::Version{ 0, 0, 0 });
	if (result != OskResult::SUCCESS) {
		OSK_SHOW_TRACE();
		return (int)result;
	}

	OSK::Sprite texture{};
	RenderAPI.LoadSprite(&texture, "models/cube/td.png");

	OSK::Font fuente{};
	RenderAPI.LoadFont(fuente, "Fonts/arial.ttf", 20);

	OSK::SpriteBatch spriteBatch = RenderAPI.CreateSpriteBatch();
	spriteBatch.DrawSprite(texture);

	RenderAPI.SubmitSpriteBatch(spriteBatch);
	RenderAPI.DefaultCamera2D.UseTargetSize = false;
	texture.SpriteTransform.SetPosition({ 20, 20 });
	texture.SpriteTransform.SetScale({ -100, 100 });
	texture.SetTexCoords(0, 0, 50, 80);

	//UI
	/*OSK::UI::Panel* mainUI = new OSK::UI::Panel({ 0.0f });
	std::cout << mainUI->Children.size() << std::endl;
	mainUI->FillParent = true;

	windowAPI->SetUserInterface(mainUI);
	mainUI->SetSprite(texture);

	OSK::UI::Button* playButton = new OSK::UI::Button();
	playButton->ElementAnchor = OSK::Anchor::BOTTOM_LEFT;
	mainUI->AddElement(playButton);
	playButton->SetRectangle({ 20, 0, 50, 51 });
	playButton->SetSprite(texture);
	playButton->OnLeftClick = ([&windowAPI, &mainUI] { windowAPI->SetMouseMode(OSK::MouseInputMode::ALWAYS_RETURN); mainUI->IsActive = false; });
	playButton->OnWheelChange = [](float val) { OSK::Logger::Log(OSK::LogMessageLevels::INFO, std::to_string(val)); };
	playButton->Texto = "PLAY";
	playButton->TextAnchor = OSK::Anchor::CENTER;
	playButton->TextFont = &fuente;


	OSK::UI::Button* exitButton = new OSK::UI::Button();
	exitButton->ElementAnchor = OSK::Anchor::TOP_RIGHT;
	mainUI->AddElement(exitButton);
	exitButton->SetRectangle({ 20, 20, 50, 51 });
	exitButton->SetSprite(texture);
	exitButton->TextAnchor = OSK::Anchor::BOTTOM_RIGHT;
	exitButton->OnLeftClick = ([&windowAPI] { windowAPI->Close(); });
	exitButton->Texto = "EXIT";
	exitButton->TextFont = &fuente;*/

	//
	OSK::KeyboardState OldKS = {};
	OSK::KeyboardState NewKS = {};
	OSK::MouseState OldMS = {};
	OSK::MouseState NewMS = {};

	OSK::Vector4i vec(0, 1, 2, 3);
	OSK::Vector4f a= vec.ToVector4f();

	windowAPI->UpdateKeyboardState(OldKS);
	windowAPI->UpdateKeyboardState(NewKS);
	windowAPI->UpdateMouseState(OldMS);
	windowAPI->UpdateMouseState(NewMS);
	double deltaTime = 0.0;

	float FPS = 0.0f;
	float totalDeltaTime = 0.0f;
	int count = 0;
	while (!windowAPI->WindowShouldClose()) {
		double startTime = windowAPI->GetTime();
		windowAPI->PollEvents();
		windowAPI->UpdateKeyboardState(NewKS);
		windowAPI->UpdateMouseState(NewMS);

		if (NewKS.IsKeyDown(OSK::Key::F11) && OldKS.IsKeyUp(OSK::Key::F11))
			windowAPI->SetFullscreen(!windowAPI->IsFullscreen);

		if (NewKS.IsKeyDown(OSK::Key::UP))
			texture.SpriteTransform.AddPosition(OSK::Vector2(0, -150) * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::DOWN))
			texture.SpriteTransform.AddPosition(OSK::Vector2(0, 150) * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::LEFT))
			texture.SpriteTransform.AddPosition(OSK::Vector2(-150, 0) * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::RIGHT))
			texture.SpriteTransform.AddPosition(OSK::Vector2(150, 0) * deltaTime);

		if (NewKS.IsKeyDown(OSK::Key::W))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(RenderAPI.DefaultCamera3D.Front * 1 * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::S))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(-RenderAPI.DefaultCamera3D.Front * 1 * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::A))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(-RenderAPI.DefaultCamera3D.Right * 1 * deltaTime);
		if (NewKS.IsKeyDown(OSK::Key::D))
			RenderAPI.DefaultCamera3D.CameraTransform.AddPosition(RenderAPI.DefaultCamera3D.Right * 1 * deltaTime);

		mouseVar_t deltaX = NewMS.PositionX - OldMS.PositionX;
		mouseVar_t deltaY = NewMS.PositionY - OldMS.PositionY;
		RenderAPI.DefaultCamera3D.Girar(deltaX, -deltaY);

		if (NewKS.IsKeyDown(OSK::Key::V) && OldKS.IsKeyUp(OSK::Key::V))
			RenderAPI.SetPresentMode(OSK::PresentMode::VSYNC);
		if (NewKS.IsKeyDown(OSK::Key::B) && OldKS.IsKeyUp(OSK::Key::B))
			RenderAPI.SetPresentMode(OSK::PresentMode::VSYNC_TRIPLE_BUFFER);

		if (NewKS.IsKeyDown(OSK::Key::ENTER) && OldKS.IsKeyUp(OSK::Key::ENTER)) {
			OSK::Vector2 mousePos = RenderAPI.DefaultCamera2D.PointInWindowToPointInWorld(NewMS.GetMouseRectangle().GetRectanglePosition());
			std::cout << "X: " << mousePos.X << "; Y: " << mousePos.Y << std::endl;
		}

		totalDeltaTime += deltaTime;
		count++;
		if (totalDeltaTime > 1.0f) {
			FPS = count;
			count = 0;

			totalDeltaTime = 0.0f;
		}

		//Draw();
		{
			spriteBatch.Clear();
			spriteBatch.DrawSprite(texture);
			spriteBatch.DrawString(fuente, "FPS: " + std::to_string((int)FPS), 0.75f, OSK::Vector2(0, 5), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
			spriteBatch.DrawString(fuente, "OSKengine " + std::string(OSK::ENGINE_VERSION), 0.75f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);

			spriteBatch.DrawString(fuente, "3D POS: " + OSK::ToString(RenderAPI.DefaultCamera3D.CameraTransform.Position.ToGLM()), 0.75f, OSK::Vector2(0, 25), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
			spriteBatch.DrawString(fuente, "3D ROT: " + OSK::ToString(RenderAPI.DefaultCamera3D.CameraTransform.Rotation.ToGLM()), 0.75f, OSK::Vector2(0, 40), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
			spriteBatch.DrawString(fuente, "3D FRONT: " + OSK::ToString(RenderAPI.DefaultCamera3D.Front.ToGLM()), 0.75f, OSK::Vector2(0, 55), OSK::Color::WHITE(), OSK::Anchor::TOP_RIGHT);
			
			RenderAPI.SubmitSpriteBatch(spriteBatch);
		}

		RenderAPI.RenderFrame();

		OldKS = NewKS;
		OldMS = NewMS;
		double endTime = windowAPI->GetTime();
		deltaTime = endTime - startTime;
	}

	RenderAPI.Close();

	delete windowAPI;
	//delete mainUI;
	//delete playButton;
	//delete exitButton;

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

/**/

/*
#include <glad/glad.h>
#include <glfw3.h>

#include "AudioAPI.h"
#include "BaseUIElement.h"
#include "Camera3D.h"
#include "ContentAPI.h"
#include "FileIO.h"
#include "KeyboardState.h"
#include "Material.h"
#include "Model.h"
#include "Panel.h"
#include "PointLight.h"
#include "RenderAPI.h"
#include "Shader.h"
#include "SoundEntity.h"
#include "ToString.h"
#include "Button.h"
#include "Checkbox.h"
#include "ButtonCodeEnum.h"
#include "WindowAPI.h"

int xd() {

	//
	deltaTime_t CurrentTime = 0.0f;
	deltaTime_t DeltaTime = 0.0f;
	deltaTime_t LastTime = 0.0f;
	deltaTime_t FPS = 0.0f;

	bool useFXAA = 1;

	OSK::Logger::Start();

	//Ventana
	OSK::WindowAPI windowAPI = OSK::WindowAPI();
	windowAPI.SetOpenGLVersion(3, 3);

	//Crear ventana { sizeX, sizeY, título }
	result_t result = windowAPI.SetWindow(1280, 720, "OSKframework", OSK::GraphicsAPI::OPENGL);
	if (!result) {
		OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear ventana", __LINE__);
#ifdef OSK_RELEASE
		OSK::Log(OSK::LogMessageLevels::COMMAND, OSK_LOG_COMMAND_SAVE_LOG);
#endif
	}

	//Iniciar OpenGL
	result = windowAPI.LoadOpenGL();
	if (!result) {
		OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "iniciar OpenGL", __LINE__);
#ifdef OSK_RELEASE
		OSK::Log(OSK::LogMessageLevels::COMMAND, OSK_LOG_COMMAND_SAVE_LOG);
#endif
	}

	windowAPI.SetMouseMode(OSK::MouseInputMode::NORMAL);
	windowAPI.SetMouseMovementMode(OSK::MouseMovementMode::RAW);

	//Clase para el renderizado
	OSK::RenderAPI renderAPI = OSK::RenderAPI();
	renderAPI.SetWindowAPI(&windowAPI);
	renderAPI.Init();

	//Material.
	OSK::Material material = { 0, 1, OSK::Color(1.0f), 16.0f };

	//Resource manager
	OSK::ContentAPI contentAPI = OSK::ContentAPI();

	//Sistema de audio
	OSK::AudioAPI audioAPI = OSK::AudioAPI();

	OSK::SoundEntity sonido = OSK::SoundEntity();

	//Input: teclado
	OSK::KeyboardState OldKS = {};
	OSK::KeyboardState NewKS = {};

	//Input: ratón
	OSK::MouseState OldMS = {};
	OSK::MouseState NewMS = {};

	//Cámara
	OSK::OldCamera3D Camera = OSK::OldCamera3D(0.0f, 0.0f, 0.0f);
	Camera.Window = &windowAPI;
	Camera.Speed = 5;

	//Shaders
	OSK::Shader SceneShader = OSK::Shader();
	contentAPI.LoadShaderFromFile(SceneShader, "shaders/OSKengine_straight_shader/Shader", "shaders/OSKengine_straight_shader/Shader");
	renderAPI.SetSceneShader(&SceneShader);

	OSK::Shader ScreenShader = OSK::Shader();
	contentAPI.LoadShaderFromFile(ScreenShader, "shaders/OSKengine_screen_shader/Shader", "shaders/OSKengine_screen_shader/Shader");
	renderAPI.SetScreenShader(&ScreenShader);

	OSK::DirectionalLight directionalLight = OSK::DirectionalLight { OSK::Vector3(-0.7f, -0.8f, -0.4f), OSK::Color(1.0f, 1.0f, 1.0f), 1.0f };

	//Modelo
	OSK::OldModel model = OSK::OldModel(OSK::Vector3(10, 0, 0), OSK::Vector3(0.01f), OSK::Vector3(0.0f));
	//contentAPI.LoadModel(model, "models/cube/cube.obj");
	contentAPI.LoadModel(model, "models/anim2/goblin.dae");
	//contentAPI.LoadModel(model, "models/anim/boblampclean.md5mesh");

	OSK::Transform transform = OSK::Transform();
	transform.SetPosition(OSK::Vector3(0, 1, 0));
	transform.SetScale(OSK::Vector3(0.09f));

	//transform.AttachTo(&model.Skeleton.Bones[3].Transform);

	OSK::OldFont fuente = OSK::OldFont();
	contentAPI.LoadFont(fuente, "Fonts/arial.ttf", 12);

	OSK::Shader textShader = OSK::Shader();
	contentAPI.LoadShaderFromFile(textShader, "shaders/OSKengine_text_shader/Shader", "shaders/OSKengine_text_shader/Shader");
	renderAPI.SetTextShader(&textShader);

	OSK::Shader spriteShader = OSK::Shader();
	contentAPI.LoadShaderFromFile(spriteShader, "shaders/OSKengine_2D_shader/Shader", "shaders/OSKengine_2D_shader/Shader");
	renderAPI.SetSpriteShader(&spriteShader);

	//Audio
	contentAPI.LoadSound(sonido, "audio/bounce.wav");
	sonido.SetPosition(OSK::Vector3(0, 0, 0));
	sonido.SoundTransform.AttachTo(&Camera.CameraTransform);
	audioAPI.PlayAudio(sonido);


	OSK::PointLight LuzPuntual = {};
	//LuzPuntual.LightTransform.SetPosition(OSK::Vector3(0, 0, 0));
	LuzPuntual.Radius = 100;
	LuzPuntual.Color = OSK::Color(0.0f, 1.0f, 1.0f);
	LuzPuntual.Intensity = 1.0f;
	LuzPuntual.Constant = 1.0f;
	LuzPuntual.Linear = 0.09f;
	LuzPuntual.Quadratic = 0.032f;

	//LuzPuntual.LightTransform.AttachTo(&Camera.CameraTransform);

	OSK::PointLight LuzPuntual2 = {};
	//LuzPuntual2.LightTransform.SetPosition(OSK::Vector3(0, 1, 0));
	LuzPuntual2.Radius = 100;
	LuzPuntual2.Color = OSK::Color(1.0f, 0.0f, 1.0f);
	LuzPuntual2.Intensity = 1.0f;
	LuzPuntual2.Constant = 1.0f;
	LuzPuntual2.Linear = 0.09f;
	LuzPuntual2.Quadratic = 0.032f;

	OSK::PointLight LuzPuntual3 = {};
	//LuzPuntual3.LightTransform.SetPosition(OSK::Vector3(0, 0, 4));
	LuzPuntual3.Radius = 1000;
	LuzPuntual3.Color = OSK::Color(0.0f, 0.0f, 1.0f);
	LuzPuntual3.Intensity = 1.0f;
	LuzPuntual3.Constant = 1.0f;
	LuzPuntual3.Linear = 0.09f;
	LuzPuntual3.Quadratic = 0.032f;
	//LuzPuntual3.LightTransform.AttachTo(&model.ModelTransform);

	OSK::Color fpsColor = OSK::Color(1.0f, 1.0f, 1.0f) * 0.5f;
	
	renderAPI.AddPointLight(&LuzPuntual);
	renderAPI.AddPointLight(&LuzPuntual2);
	renderAPI.AddPointLight(&LuzPuntual3);

	renderAPI.SetCamera3D(&Camera);

	const std::array<std::string, 6> skyboxFaces = { 
		"skybox/right",
		"skybox/left",
		"skybox/top",
		"skybox/bottom",
		"skybox/front",
		"skybox/back"
	};

	OSK::Skybox skybox = {};
	contentAPI.LoadSkybox(skybox, skyboxFaces);

	OSK::Shader skyboxShader = OSK::Shader();
	contentAPI.LoadShaderFromFile(skyboxShader, "shaders/OSKengine_skybox_shader/Shader", "shaders/OSKengine_skybox_shader/Shader");

	renderAPI.SetSkybox(skybox);
	renderAPI.SetSkyboxShader(&skyboxShader);

	model.Skeleton.PlayAnimation(new OSK::Animation("main", 0, 245, 0.34, 10, true));
	
	OSK::UI::Panel* mainUI = new OSK::UI::Panel(OSK::Vector4(0.0f));
	mainUI->ImageTexture = &model.Diffuse;
	mainUI->FillParent = true;

	windowAPI.SetUserInterface(mainUI);

	OSK::UI::Button* playButton = new OSK::UI::Button();
	playButton->ImageTexture = &model.Specular;
	playButton->ElementAnchor = OSK::Anchor::CENTER_RIGHT;
	playButton->OnLeftClick = ([&windowAPI, &mainUI] { windowAPI.SetMouseMode(OSK::MouseInputMode::ALWAYS_RETURN); mainUI->IsActive = false; });
	playButton->OnWheelChange = [](float val) { OSK::Logger::Log(OSK::LogMessageLevels::INFO, std::to_string(val)); };
	playButton->Texto = "PLAY";
	playButton->TextAnchor = OSK::Anchor::CENTER;
	playButton->OnMouseHover = [&sonido, &audioAPI]() { audioAPI.PlayAudio(sonido); };
	playButton->OnMouseUnhover = []() { OSK::Logger::Log(OSK::LogMessageLevels::INFO, "NOT MouseHover!"); };
	playButton->OldTextFont = &fuente;

	mainUI->AddElement(playButton);

	OSK::UI::Button* exitButton = new OSK::UI::Button();
	exitButton->ImageTexture = &model.Specular;
	exitButton->ElementAnchor = OSK::Anchor::TOP_RIGHT;
	exitButton->TextAnchor = OSK::Anchor::CENTER;
	exitButton->OnLeftClick = ([&windowAPI] { windowAPI.Close(); });
	exitButton->Texto = "EXIT";
	exitButton->OldTextFont = &fuente;

	mainUI->AddElement(exitButton);

	OSK::UI::Checkbox* checkbox = new OSK::UI::Checkbox(OSK::Vector4(50, 50, 50, 50));
	checkbox->LinkedValue = &useFXAA;
	checkbox->ImageTexture = &model.Specular;
	mainUI->AddElement(checkbox);

	audioAPI.SetCamera3D(&Camera);

	//Main loop
	while (!windowAPI.WindowShouldClose()) {

		CurrentTime = windowAPI.GetTime();
		DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;
		FPS = 1.0f / DeltaTime;

		windowAPI.UpdateKeyboardState(NewKS);
		windowAPI.UpdateMouseState(NewMS);

		cameraVar_t scrollOffset = NewMS.ScrollY - OldMS.ScrollY;
		Camera.AddFoV(scrollOffset);

		mouseVar_t posOffsetX = NewMS.PositionX - OldMS.PositionX;
		mouseVar_t posOffsetY = NewMS.PositionY - OldMS.PositionY;
		Camera.Girar(posOffsetX, -posOffsetY);

		audioAPI.Update();
		
		if (OldKS.IsKeyDown(OSK::Key::ESCAPE) && NewKS.IsKeyUp(OSK::Key::ESCAPE)) {
			//mainUI->IsActive = true;
			windowAPI.SetMouseMode(OSK::MouseInputMode::NORMAL);
			windowAPI.CenterMouse();
		}
		
		if (OldKS.IsKeyUp(OSK::Key::F1) && NewKS.IsKeyDown(OSK::Key::F1))
			renderAPI.RenderOutputImage = OSK::RenderOutput::SCENE;
		if (OldKS.IsKeyUp(OSK::Key::F2) && NewKS.IsKeyDown(OSK::Key::F2))
			renderAPI.RenderOutputImage = OSK::RenderOutput::NORMALS;
		if (OldKS.IsKeyUp(OSK::Key::F3) && NewKS.IsKeyDown(OSK::Key::F3))
			renderAPI.RenderOutputImage = OSK::RenderOutput::POSITION;
		if (OldKS.IsKeyUp(OSK::Key::F4) && NewKS.IsKeyDown(OSK::Key::F4))
			windowAPI.SetFullscreen(!windowAPI.IsFullscreen);
		

		if (NewMS.IsButtonDown(OSK::ButtonCode::BUTTON_LEFT) && OldMS.IsButtonUp(OSK::ButtonCode::BUTTON_LEFT)) {
			//useFXAA = !useFXAA;
		}

		if (NewKS.IsKeyDown(OSK::Key::P) && OldKS.IsKeyDown(OSK::Key::P))
			audioAPI.PauseAudio(sonido);

		if (NewKS.IsKeyDown(OSK::Key::W))
			Camera.Move(OSK::Directions::FORWARD, DeltaTime);
		if (NewKS.IsKeyDown(OSK::Key::S))
			Camera.Move(OSK::Directions::BACKWARDS, DeltaTime);
		if (NewKS.IsKeyDown(OSK::Key::A))
			Camera.Move(OSK::Directions::LEFT, DeltaTime);
		if (NewKS.IsKeyDown(OSK::Key::D))
			Camera.Move(OSK::Directions::RIGHT, DeltaTime);

		if (NewKS.IsKeyDown(OSK::Key::F11) && OldKS.IsKeyUp(OSK::Key::F11)) {
			windowAPI.SetFullscreen(!windowAPI.IsFullscreen);
		}

		if (NewKS.IsKeyDown(OSK::Key::Z)) {
			model.ModelTransform.AddRotation(OSK::Vector3(-1, 0, 0));
		}
		if (NewKS.IsKeyDown(OSK::Key::X)) {
			model.ModelTransform.AddRotation(OSK::Vector3(0, 1, 0));
		}
		if (NewKS.IsKeyDown(OSK::Key::C)) {
			model.ModelTransform.AddRotation(OSK::Vector3(0, 0, 1));
		}

		if (NewKS.IsKeyDown(OSK::Key::Q) && OldKS.IsKeyDown(OSK::Key::Q))
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, OSK::ToString(Camera.CameraTransform.Rotation));

		if (FPS > 55)
			fpsColor.Update(0.0f, 0.0f, 1.0f);
		else if (FPS > 29)
			fpsColor.Update(0.0f, 1.0f, 0.0f);
		else
			fpsColor.Update(1.0f, 0.0f, 0.0f);

		OldKS = NewKS;
		OldMS = NewMS;

		//Render
		renderAPI.StartFirstRenderPass();
		renderAPI.Clear(OSK::Color(0.0f, 0.0f, 0.0f));

		SceneShader.SetDirectionalLight(directionalLight);
		SceneShader.SetMaterial(material);

		model.Skeleton.Update(DeltaTime);
		renderAPI.DrawModel(model, DeltaTime);
		//renderAPI.DrawModel(model, transform, DeltaTime);

		renderAPI.DrawSkybox();

		renderAPI.StartSpriteRenderPass();
		//renderAPI.DrawTexture(model.Diffuse, OSK::Vector2(30.0f), OSK::Vector2(25, 25), OSK::Color(1.0f, 1.0f, 1.0f, 1.2f) * 0.15f, OSK::Vector4(-1.0f, -1.0f, 1.0f, 1.0f), model.ModelTransform.GlobalRotation.Y, OSK::Vector2(250 / 2, 250 / 2));
//		renderAPI.DrawTexture(model.Diffuse, OSK::Vector2(30.0f), OSK::Vector2(25, 25), OSK::Color(1.0f, 1.0f, 1.0f, 1.2f) * 0.15f);

		renderAPI.DrawUserInterface();

		renderAPI.StartSecondRenderPass();
		ScreenShader.SetBool("UseAA", useFXAA);
		renderAPI.RenderScreen();

		renderAPI.StartTextRenderPass();
		renderAPI.DrawString(fuente, 1.0f, std::string(OSK::ENGINE_NAME) + " " + std::string(OSK::ENGINE_VERSION_ALPHANUMERIC), OSK::Vector2(15, 15), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT, 0, 20);
		renderAPI.DrawString(fuente, 0.8f, std::string(OSK::ENGINE_VERSION) + " (" + std::to_string(OSK::ENGINE_VERSION_TOTAL) + " / " + OSK::ToString(windowAPI.UsedGraphicsAPI) + ")", OSK::Vector2(20, 20), OSK::Color(0.3f, 0.7f, 0.9f) * 0.5f, OSK::Anchor::BOTTOM_LEFT);
		renderAPI.DrawString(fuente, 1.0f, "FPS: " + std::to_string((int)FPS + 1), OSK::Vector2(20, 20), fpsColor * 1.0f, OSK::Anchor::TOP_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT, 0, 20);

		windowAPI.SwapBuffers();
		windowAPI.PollEvents();
	}

#ifdef OSK_RELEASE
	OSK::Log(OSK::LogMessageLevels::COMMAND, OSK_LOG_COMMAND_SAVE_LOG);
#endif

	//delete mainUI;
	//delete playButton;
	//delete exitButton;
	//delete checkbox;

	windowAPI.~WindowAPI();
	renderAPI.~RenderAPI();
	contentAPI.~ContentAPI();

	OSK::Logger::Close();

	return 0;
}

int main() {

	try {
		xd();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		
		return -1;
	}

	return 0;
}

/**/
#endif