#include "Game.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Vertex2D.h"
#include "Vertex.h"
#include "IGpuMemoryAllocator.h"
#include "Sprite.h"
#include "InputManager.h"

#include "IKeyboardInput.h"
#include "IMouseInput.h"
#include "IGamepadInput.h"

#include "GameExceptions.h"
#include "GpuImageLayout.h"

#include "FileIO.h"
#include <json.hpp>
#include "IGpuImage.h"

#include "Platforms.h"
#ifdef OSK_ANDROID
#include <android_native_app_glue.h>
#endif

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IGame::IGame(GAME::DefaultContentProfile defaultContentProfile) : m_defaultContentProfile(defaultContentProfile) {

}

void IGame::RegisterAssets() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterComponents() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterSystems() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterConsoleCommands() {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnCreate() {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnTick_BeforeEcs(TDeltaTime) {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnTick_AfterEcs(TDeltaTime) {
	// Sobreescrito en clase Game del juego.
}

void IGame::BuildFrame() {
	auto commandList = Engine::GetRenderer()->GetMainCommandList();

	commandList->SetGpuImageBarrier(
		Engine::GetRenderer()->_GetSwapchain()->GetImage(Engine::GetRenderer()->GetCurrentFrameIndex()),
		GpuImageLayout::PRESENT,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE));
}

void IGame::OnExit() {
	// Sobreescrito en clase Game del juego.
}

#ifdef OSK_ANDROID
const char* CmdToStr2(int32_t cmd) {
	switch (cmd) {
	case  0: return "APP_CMD_INPUT_CHANGED";
	case  1: return "APP_CMD_INIT_WINDOW"; // a new ANativeWindow is ready for use
	case  2: return "APP_CMD_TERM_WINDOW"; // the existing ANativeWindow needs to be terminated
	case  3: return "APP_CMD_WINDOW_RESIZED";
	case  4: return "APP_CMD_WINDOW_REDRAW_NEEDED";
	case  5: return "APP_CMD_CONTENT_RECT_CHANGED";
	case  6: return "APP_CMD_GAINED_FOCUS";
	case  7: return "APP_CMD_LOST_FOCUS";
	case  8: return "APP_CMD_CONFIG_CHANGED";
	case  9: return "APP_CMD_LOW_MEMORY";
	case 10: return "APP_CMD_START";
	case 11: return "APP_CMD_RESUME";
	case 12: return "APP_CMD_SAVE_STATE";
	case 13: return "APP_CMD_PAUSE";
	case 14: return "APP_CMD_STOP";
	case 15: return "APP_CMD_DESTROY";
	case 16: return "APP_CMD_PAUSE";

	default: return "";
	};
};
#endif


#ifdef OSK_ANDROID
void IGame::_Run(struct android_app* app) {
	IO::FileIO::SetAssetManager(app->activity->assetManager);
#define OSK_EXTRA_ARGS , app
#else
#define OSK_EXTRA_ARGS 
void IGame::_Run() {
#endif
	nlohmann::json engineConfig = nlohmann::json::parse(FileIO::ReadFromFile("engine_config.json"));

	{
		const auto& graphicsApi = engineConfig["graphics_backend"];
		using enum OSK::GRAPHICS::RenderApiType;

		if (graphicsApi == "VULKAN")
			Engine::Create(VULKAN OSK_EXTRA_ARGS);
		else {
			Engine::Create(VULKAN OSK_EXTRA_ARGS);
		}
	}
	
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Iniciado GAME.");

	CreateWindow();
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Iniciada VENTANA.");
	Engine::RegisterBuiltinVertices();
	SetupEngine();
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "SETUP ENGINE.");

	Engine::RegisterBuiltinAssets(m_defaultContentProfile);
	Engine::RegisterBuiltinComponents(m_defaultContentProfile);
	Engine::RegisterBuiltinSystems(m_defaultContentProfile);
	Engine::RegisterBuiltinEvents();
	Engine::RegisterBuiltinJobs();
	Engine::RegisterBuiltinShaderPasses(m_defaultContentProfile);
	Engine::RegisterBuiltinConsoleCommands();

	m_rootUiElement = MakeUnique<UI::FreeContainer>(Engine::GetDisplay()->GetResolution().ToVector2f());
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Registros.");


	const DynamicArray<Vertex2D> vertices2d = {
		{ { 0, 0 }, { 0, 0 } },
		{ { 0, 1 }, { 0, 1 } },
		{ { 1, 0 }, { 1, 0 } },
		{ { 1, 1 }, { 1, 1 } }
	};

	const DynamicArray<TIndexSize> indices2d = {
		0, 1, 2, 1, 2, 3
	};

	Sprite::globalVertexBuffer = Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices2d, Vertex2D::GetVertexInfo(), GpuQueueType::MAIN);
	Sprite::globalIndexBuffer  = Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices2d, GpuQueueType::MAIN);

	OSK_ASSERT(Engine::GetDisplay()->IsOpen(), WindowNotCreatedException());
	// OSK_ASSERT(Engine::GetRenderer()->IsOpen(), RenderedNotCreatedException());

	RegisterAssets();
	RegisterComponents();
	RegisterSystems();
	RegisterConsoleCommands();
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Registros finales.");

	OnCreate();
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "OnCreate.");

	Engine::GetRenderer()->PresentFrame();

	while (Engine::GetDisplay()->IsOpen()) {
		Engine::Update();

		const TDeltaTime startTime = Engine::GetCurrentTime();

		Engine::GetDisplay()->Update();

		Engine::GetInput()->Update();
		Engine::GetInputManager()->_Update(*Engine::GetInput());

		OnTick_BeforeEcs(m_deltaTime);
		UpdateUi();
		if (const auto* mouse = GetMouse())			OnTick_Mouse(*mouse);
		if (const auto* keyboard = GetKeyboard())	OnTick_Keyboard(*keyboard);
		Engine::GetEcs()->OnTick(m_deltaTime);
		OnTick_AfterEcs(m_deltaTime);

		if (m_renderGraph.HasValue()) {
			m_renderGraph->Execute(Engine::GetRenderer()->GetMainCommandList());
		}

		Engine::GetEcs()->OnRender(Engine::GetRenderer()->GetMainCommandList());

		BuildFrame();

		Engine::GetEcs()->_ClearEventQueues();

		Engine::GetRenderer()->PresentFrame();

		HandleResizeEvents();
		UpdateFps(m_deltaTime);

		const TDeltaTime endTime = Engine::GetCurrentTime();
		m_deltaTime = endTime - startTime;

#ifdef OSK_ANDROID
		int events = 1;
		while (events > 0) {
			struct android_poll_source* source;

			int event = ALooper_pollOnce(0, nullptr, &events, (void**)&source);

			if (events > 0) {
				if (source != NULL) {
					source->process(app, source);
				}

				if (app->destroyRequested != 0) {
					return;
				}
			}
		}
#endif // OSK_ANDROID

	}

	m_rootUiElement.Delete();

	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "END.");
	Engine::GetRenderer()->WaitForCompletion();

	Sprite::globalVertexBuffer.Delete();
	Sprite::globalIndexBuffer.Delete();

	if (m_renderGraph.HasValue()) {
		m_renderGraph.Delete();
	}

	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "EXIT.");
	OnExit();

	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "CLOSE.");
	Engine::Close();
}

#ifdef OSK_ANDROID
void IGame::AndroidHandleAppCommand(android_app* app, int32_t cmd) {
	auto* game = static_cast<IGame*>(app->userData);

	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "AndroidHandleAppCommand: Evento: ", CmdToStr2(cmd));

	switch (cmd) {

	case APP_CMD_INIT_WINDOW:
		Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Recreando...");
		game->RecreateWindow();
		break;

	case APP_CMD_TERM_WINDOW:
	case APP_CMD_DESTROY:
		game->Exit();
		break;

	};
}
#endif

void IGame::UpdateFps(TDeltaTime deltaTime) {
	m_framerateCountTimer += deltaTime;
	m_frameCount++;
	if (m_framerateCountTimer > 1.0f) {
		m_currentFps = m_frameCount;
		m_frameCount = 0;
		m_framerateCountTimer = 0.0f;
	}
}

void IGame::OnTick_Keyboard(const IO::IKeyboardInput& keyboard) {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnTick_Mouse(const IO::IMouseInput& mouse) {
	// Sobreescrito en clase Game del juego.
}

void IGame::UpdateUi() {
	if (const auto* mouse = GetMouse()) {
		const bool isPressed = mouse->GetMouseState().IsButtonDown(IO::MouseButton::BUTTON_LEFT);
		const Vector2f position = mouse->GetMouseState().GetPosition().ToVector2f();

		GetRootUiElement().UpdateByCursor(position, isPressed);
	}

	if (const auto* keyboard = GetKeyboard()) {
		GetRootUiElement().UpdateByKeyboard(
			keyboard->GetPreviousKeyboardState(),
			keyboard->GetKeyboardState());
	}
}

IO::IKeyboardInput* IGame::GetKeyboard() {
	IO::IKeyboardInput* keyboard = nullptr;
	Engine::GetInput()->QueryInterface(IUUID::IKeyboardInput, (void**)&keyboard);
	return keyboard;
}

IO::IMouseInput* IGame::GetMouse() {
	IO::IMouseInput* mouse = nullptr;
	Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouse);
	return mouse;
}

IO::IGamepadInput* IGame::GetGamepad() {
	IO::IGamepadInput* gamepad = nullptr;
	Engine::GetInput()->QueryInterface(IUUID::IGamepadInput, (void**)&gamepad);
	return gamepad;
}

IO::ITouchInput* IGame::GetTouchs() {
	IO::ITouchInput* touch = nullptr;
	Engine::GetInput()->QueryInterface(IUUID::ITouchInput, (void**)&touch);
	return touch;
}

void IGame::Exit() {
	Engine::GetDisplay()->Close();
}

void IGame::OnWindowResize(const Vector2ui& size) {
	// Sobreescrito en clase Game del juego.
}

void IGame::RecreateWindow() {
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Esperando...");
	Engine::GetRenderer()->WaitForCompletion();
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Recreando ventana...");
	CreateWindow();
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Reestableciendo display del renderizador...");
	Engine::GetRenderer()->ResetDisplay(*Engine::GetDisplay());
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "OnWindowResize...");
	OnWindowResize(Engine::GetDisplay()->GetResolution());
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "FIN");

	// Readquirimos las imágenes del renderizador.
	Engine::GetRenderer()->WaitForCompletion();
}

USize32 IGame::GetFps() const {
	return m_currentFps;
}

const UI::IContainer& IGame::GetRootUiElement() const {
	return *m_rootUiElement.GetPointer();
}

UI::IContainer& IGame::GetRootUiElement() {
	return *m_rootUiElement.GetPointer();
}

void IGame::HandleResizeEvents() {
#ifdef OSK_PC_ANY
	const auto& resizeEvents = Engine::GetEcs()->GetEventQueue<IDisplay::ResolutionChangedEvent>();

	for (const auto& event : resizeEvents) {
		m_rootUiElement->SetSize(event.newResolution.ToVector2f());
		m_rootUiElement->Rebuild();

		OnWindowResize(event.newResolution);
	}
#endif // OSK_PC_ANY
}

void IGame::SetRenderGraph(UniquePtr<GRAPHICS::RenderGraph>&& renderGraph) {
	m_renderGraph = std::move(renderGraph);
}

GRAPHICS::RenderGraph* IGame::GetRenderGraph() {
	return m_renderGraph.GetPointer();
}
