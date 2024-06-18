#include "EditorUi.h"

#include "EntityComponentSystem.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"
#include "Texture.h"

#include "EditorObjectList.h"
#include "EditorSystemList.h"
#include "EditorHeader.h"
#include "EditorBottomBar.h"

#include "EditorUiConstants.h"

// module OSKengine.Editor.Ui;

// import :BottomBar;

OSK::Editor::UI::EditorUi::EditorUi(const Vector2f& size) : OSK::UI::BorderLayout(size) {
	auto editorFont = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	const USize32 fontSize = 14;

	// Header
	AddChild_InPosition(new EditorHeader(Vector2f::Zero), Position::NORTH);

	// Bottom bar
	{
		m_bottomBar = new EditorBottomBar(Vector2f::Zero);
		AddChild_InPosition(m_bottomBar, Position::SOUTH);
	}

	// Object list
	{
		m_objectListPanel = new Editor::UI::ObjectList({ 300.0f, 500.0f });

		m_objectListPanel->SetAnchor(OSK::UI::Anchor::BOTTOM | OSK::UI::Anchor::RIGHT);

		m_objectListPanel->SetFont(editorFont);
		m_objectListPanel->SetFontSize(fontSize);

		m_objectListPanel->AdjustSizeToChildren();

		AddChild_InPosition(m_objectListPanel, Position::WEST);
	}

	// System list
	{
		m_systemListPanel = new Editor::UI::SystemList({ 300.0f, 500.0f });
		m_systemListPanel->SetAnchor(OSK::UI::Anchor::BOTTOM | OSK::UI::Anchor::RIGHT);

		m_systemListPanel->SetFont(editorFont);
		m_systemListPanel->SetFontSize(fontSize);

		m_systemListPanel->AdjustSizeToChildren();

		AddChild_InPosition(m_systemListPanel, Position::EAST);
	}
}

void OSK::Editor::UI::EditorUi::Update(const OSK::ECS::EntityComponentSystem* ecs, TDeltaTime deltaTime) {
	// Objects.
	{
		auto livingObjects = ecs->GetLivingObjects();

		m_objectListPanel->ClearObjects();
		m_objectListPanel->SetObjects(livingObjects);
	}

	// Systems
	{
		auto livingSystems = ecs->GetAllSystems();

		m_systemListPanel->ClearSystems();
		m_systemListPanel->SetSystems(livingSystems.GetFullSpan());
	}

	// Bottom bar
	{
		m_bottomBar->Update(deltaTime);
	}
}
