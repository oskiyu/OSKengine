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
#include "IComponentView.h"

// module OSKengine.Editor.Ui;

// import :BottomBar;

OSK::Editor::UI::EditorUi::EditorUi(const Vector2f& size, OSK::Editor::Editor* editor) : OSK::UI::BorderLayout(size) {
	OSK_ASSERT(editor, InvalidArgumentException("El editor no debe ser null."));

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
		m_objectListPanel = new OSK::Editor::UI::ObjectList({ 300.0f, 500.0f }, editor);

		m_objectListPanel->SetAnchor(OSK::UI::Anchor::BOTTOM | OSK::UI::Anchor::RIGHT);

		m_objectListPanel->AdjustSizeToChildren();

		AddChild_InPosition(m_objectListPanel, Position::WEST);
	}

	// System list
	{
		m_systemListPanel = new OSK::Editor::UI::SystemList({ 300.0f, 500.0f }, editor);
		m_systemListPanel->SetAnchor(OSK::UI::Anchor::BOTTOM | OSK::UI::Anchor::RIGHT);

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

void OSK::Editor::UI::EditorUi::ClearAllComponentViews() {
	m_objectListPanel->ClearAllComponentViews();
}

void OSK::Editor::UI::EditorUi::AddComponentView(OwnedPtr<OSK::Editor::Views::IComponentView> view) {
	m_objectListPanel->AddComponentView(view);
	m_objectListPanel->Rebuild();
}

void OSK::Editor::UI::EditorUi::SetSystemPropertiesView(OwnedPtr<OSK::Editor::Views::ISystemView> view) {
	m_systemListPanel->SetSystemPropertiesView(view);
}

void OSK::Editor::UI::EditorUi::ClearSystemPropertiesView() {
	m_systemListPanel->ClearSystemPropertiesView();
}

void OSK::Editor::UI::EditorUi::SetSelectedSystem(ECS::ISystem* system) {
	m_systemListPanel->SetSelectedSystem(system);
}
