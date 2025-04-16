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

// Controladores de componentes del editor.
#include "CameraComponentController3D.h"
#include "CameraComponentView.h"

#include "PhysicsComponentController3D.h"
#include "PhysicsComponentView.h"

#include "TransformComponentController3D.h"
#include "TransformComponentView3D.h"

#include "ModelComponentController3D.h"
#include "ModelComponentView3D.h"

// Controladores de sistemas del editor.
#include "PhysicsSystemController.h"
#include "PhysicsSystemView.h"

#include "SkyboxRenderSystemController.h"
#include "SkyboxRenderSystemView.h"

#include "DeferredRenderSystemController.h"
#include "DeferredRenderSystemView.h"

#include "ColliderRenderSystemController.h"
#include "ColliderRenderSystemView.h"

#include "RenderBoundsRenderSystemController.h"
#include "RenderBoundsRenderSystemView.h"


void OSK::IDebugGame::RegisterConsoleCommands() {
	IGame::RegisterConsoleCommands();

	Engine::GetCommandExecutor()->RegisterCommand(UniquePtr<EditorShowCommand>(m_editorShowCommand = new EditorShowCommand()));
	Engine::GetCommandExecutor()->RegisterCommand(UniquePtr<EditorHideCommand>(m_editorHideCommand = new EditorHideCommand()));
	Engine::GetCommandExecutor()->RegisterCommand(MakeUnique<ExitCommand>(this));
}

void OSK::IDebugGame::OnCreate() {
	IGame::OnCreate();

	const auto font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/Fonts/font1.json");
		
	// Editor.
	m_editor = MakeUnique<Editor::Editor>(
		&GetRootUiElement(),
		Engine::GetDisplay()->GetResolution().ToVector2f());

	m_editor->RegisterComponentController<Editor::Controllers::CameraComponentController3D>();
	m_editor->RegisterComponentView<Editor::Views::CameraComponentView>();

	m_editor->RegisterComponentController<Editor::Controllers::PhysicsComponentController3D>();
	m_editor->RegisterComponentView<Editor::Views::PhysicsComponentView>();

	m_editor->RegisterComponentController<Editor::Controllers::TransformComponentController3D>();
	m_editor->RegisterComponentView<Editor::Views::TransformComponentView3D>();

	m_editor->RegisterComponentController<Editor::Controllers::ModelComponentController3D>();
	m_editor->RegisterComponentView<Editor::Views::ModelComponentView3D>();


	m_editor->RegisterSystemController<Editor::Controllers::PhysicsSystemController>();
	m_editor->RegisterSystemView<Editor::Views::PhysicsSystemView>();

	m_editor->RegisterSystemController<Editor::Controllers::SkyboxRenderSystemController>();
	m_editor->RegisterSystemView<Editor::Views::SkyboxRenderSystemView>();

	m_editor->RegisterSystemController<Editor::Controllers::DeferredRenderSystemController>();
	m_editor->RegisterSystemView<Editor::Views::DeferredRenderSystemView>();

	m_editor->RegisterSystemController<Editor::Controllers::ColliderRenderSystemController>();
	m_editor->RegisterSystemView<Editor::Views::ColliderRenderSystemView>();

	m_editor->RegisterSystemController<Editor::Controllers::RenderBoundsRenderSystemController>();
	m_editor->RegisterSystemView<Editor::Views::RenderBoundsRenderSystemView>();


	// Console
	m_console = new UI::Console({ 500.0f, 200.0f });
	m_console->SetAnchor(UI::Anchor::BOTTOM | UI::Anchor::LEFT);
	m_console->SetFont(font);
	m_console->SetFontSize(19);
	m_console->SetInvisible();

	GetRootUiElement().AddChild(UI::Console::Name, UniquePtr<OSK::UI::IElement>(m_console));


	m_editorShowCommand->editor = m_editor->GetUi();
	m_editorHideCommand->editor = m_editor->GetUi();

	m_editor->GetUi()->SetVisible();
}

void OSK::IDebugGame::OnTick_BeforeEcs(TDeltaTime deltaTime) {
	// Input.
	IO::IKeyboardInput* keyboard = nullptr;
	Engine::GetInput()->QueryInterface(IUUID::IKeyboardInput, (void**)&keyboard);

	// Console
	if (keyboard && keyboard->IsKeyStroked(IO::Key::APOSTROPHE)) {
		m_console->SetVisibility(!m_console->IsVisible());
	}

	// Editor.
	m_editor->Update(Engine::GetEcs(), deltaTime);
}

bool OSK::IDebugGame::IsInConsole() const {
	return m_console->IsVisible();
}
