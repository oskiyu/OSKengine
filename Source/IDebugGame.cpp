#include "IDebugGame.h"

#include "OSKengine.h"

// Assets.
#include "AssetManager.h"
#include "Font.h"
#include "Texture.h"

// Interfaz de usuario.
#include "UiAnchor.h"

// Interfaces de debug/desarrollo.
#include "EditorUi.h"
#include "UiConsole.h"

// Comandos.
#include "ConsoleCommandExecutor.h"
#include "EditorCommands.h"
#include "ExitCommand.h"

// Input.
#include "IKeyboardInput.h"
#include "IQueryInterface.h"

// Tipos.
#include "Color.hpp"
#include "NumericTypes.h"
#include "KeyboardState.h"


void OSK::IDebugGame::RegisterConsoleCommands() {
	IGame::RegisterConsoleCommands();

	Engine::GetCommandExecutor()->RegisterCommand(m_editorShowCommand = new EditorShowCommand());
	Engine::GetCommandExecutor()->RegisterCommand(m_editorHideCommand = new EditorHideCommand());
	Engine::GetCommandExecutor()->RegisterCommand(new ExitCommand(this));
}

void OSK::IDebugGame::OnCreate() {
	IGame::OnCreate();

	const auto uiTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");
	const auto uiView = &uiTexture->GetTextureView2D();
	const auto font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/Fonts/font1.json");


	// Editor
	auto editorUi = new Editor::UI::EditorUi(OSK::Engine::GetDisplay()->GetResolution().ToVector2f());
	editorUi->SetAnchor(UI::Anchor::FULLY_CENTERED);
	editorUi->SetKeepRelativeSize(true);
	editorUi->Rebuild();
	editorUi->SetInvisible();

	GetRootUiElement().AddChild(Editor::UI::EditorUi::Name, editorUi);


	// Console
	m_console = new UI::Console({ 500.0f, 200.0f });
	m_console->SetAnchor(UI::Anchor::BOTTOM | UI::Anchor::LEFT);
	m_console->SetFont(font);
	m_console->SetFontSize(19);
	m_console->GetSprite().SetImageView(uiView);
	m_console->GetSprite().color = Color(0.3f, 0.3f, 0.4f, 1.0f);
	m_console->SetInvisible();

	GetRootUiElement().AddChild(UI::Console::Name, m_console);


	m_editorShowCommand->editor = editorUi;
	m_editorHideCommand->editor = editorUi;
}

void OSK::IDebugGame::OnTick_BeforeEcs(TDeltaTime deltaTime) {
	// Input.
	IO::IKeyboardInput* keyboard = nullptr;
	Engine::GetInput()->QueryInterface(IUUID::IKeyboardInput, (void**)&keyboard);

	// Console
	if (keyboard && keyboard->IsKeyStroked(IO::Key::APOSTROPHE)) {
		m_console->SetVisibility(!m_console->IsVisible());
	}
}

bool OSK::IDebugGame::IsInConsole() const {
	return m_console->IsVisible();
}
