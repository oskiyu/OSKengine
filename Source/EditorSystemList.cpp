#include "EditorSystemList.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "Editor.h"
#include "EditorUiConstants.h"
#include "EditorPanelTitle.h"
#include "EditorSystemPropertiesPanel.h"


OSK::Editor::UI::SystemList::SystemList(const Vector2f& size, OSK::Editor::Editor* editorRef) : OSK::UI::VerticalContainer(size), m_editorRef(editorRef) {
	const Vector2f textSize = { size.x - 20.0f, 25.0f };

	SetMargin(Vector2f::Zero);
	SetPadding(Vector2f::Zero);

	// Background.
	{
		GRAPHICS::SdfDrawCall2D backgroundDrawCall{};
		backgroundDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		backgroundDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		backgroundDrawCall.mainColor = Constants::BackgroundColor;
		backgroundDrawCall.transform.SetScale(GetSize());

		m_backgroundDrawCallIndex = GetAllDrawCalls().GetSize();

		AddDrawCall(backgroundDrawCall);
	}

	// Title.
	{
		m_title = new EditorPanelTitle(textSize);
		m_title->SetText("Sistemas");

		AddChild("title", UniquePtr<OSK::UI::IElement>(m_title));
	}

	auto font = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	// Botones de sistemas.
	for (UIndex64 i = 0; i < MaxShownSystems; i++) {
		auto* view = new OSK::UI::Button(textSize);
		view->SetMargin(Vector4f(5.0f, 1.0f, 5.0f, 1.0f));
		view->SetPadding(Vector2f(5.0f));

		view->SetTextFont(font);
		view->SetTextFontSize(Constants::MainFontSize);

		{
			GRAPHICS::SdfDrawCall2D selectedDrawCall{};
			selectedDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			selectedDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			selectedDrawCall.mainColor = Constants::HoveredColor;
			selectedDrawCall.transform.SetScale(view->GetSize());

			view->GetSelectedDrawCalls().Insert(selectedDrawCall);
		}

		{
			GRAPHICS::SdfDrawCall2D pressedDrawCall{};
			pressedDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			pressedDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			pressedDrawCall.mainColor = Constants::SelectedColor;
			pressedDrawCall.transform.SetScale(view->GetSize());

			view->GetPressedDrawCalls().Insert(pressedDrawCall);
		}

		if (i % 2 == 0) {
			GRAPHICS::SdfDrawCall2D defaultDrawCall{};
			defaultDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			defaultDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			defaultDrawCall.mainColor = Constants::BackgroundAlternativeColor;
			defaultDrawCall.transform.SetScale(view->GetSize());

			view->GetDefaultDrawCalls().Insert(defaultDrawCall);
		}

		AddChild(std::to_string(i), UniquePtr<OSK::UI::IElement>(view));
		m_textViews.Insert(view);
	}


	// Properties.
	{
		m_propertiesPanel = new SystemPropertiesPanel({ size.x, 300.0f });
		m_propertiesPanel->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::CENTER_Y);

		for (auto& view : m_textViews) {
			view->SetCallback([&](bool isPressed) {
				if (isPressed) {
					this->ClearSelection();
					view->SetState(OSK::UI::Button::State::PRESSED);
					m_propertiesPanel->ClearContent();
					m_propertiesPanel->ShowContent();
					m_propertiesPanel->SetSubtitle(static_cast<std::string>(view->GetText()));
					m_editorRef->SetSelectedSystem(static_cast<std::string>(view->GetText()));
				}
				else {
					m_propertiesPanel->ClearContent();
					m_editorRef->ClearSelectedSystem();
				}
				});
		}

		AddChild(PropertiesPanel::Name, UniquePtr<OSK::UI::IElement>(m_propertiesPanel));
	}

	AdjustSizeToChildren();
}

void OSK::Editor::UI::SystemList::OnSizeChanged(const Vector2f&) {
	auto& backgroundDrawCall = GetAllDrawCalls()[m_backgroundDrawCallIndex];
	backgroundDrawCall.transform.SetScale(GetSize());
}

void OSK::Editor::UI::SystemList::ClearSystems() {
	for (OSK::UI::Button* view : m_textViews) {
		view->SetInvisible();
	}
}

void OSK::Editor::UI::SystemList::SetSystems(std::span<const OSK::ECS::ISystem*> systems) {
	for (UIndex64 i = 0; i < std::min(systems.size(), m_textViews.GetSize()); i++) {
		m_textViews[i]->SetVisible();
		m_textViews[i]->SetText(std::format("{}", systems[i]->GetName()));
	}

	Rebuild();
}

void OSK::Editor::UI::SystemList::ClearSelection() {
	for (auto* view : m_textViews) {
		if (!view->IsVisible()) {
			break;
		}

		view->SetState(OSK::UI::Button::State::DEFAULT);
	}
}

void OSK::Editor::UI::SystemList::SetSystemPropertiesView(UniquePtr<OSK::Editor::Views::ISystemView>&& view) {
	m_propertiesPanel->SetView(std::move(view));
	m_propertiesPanel->SetSubtitle((std::string)view->_GetSystemName());
	m_propertiesPanel->ShowContent();
}

void OSK::Editor::UI::SystemList::ClearSystemPropertiesView() {
	m_propertiesPanel->ClearContent();
}

void OSK::Editor::UI::SystemList::SetSelectedSystem(ECS::ISystem* system) {
	m_propertiesPanel->SetSelectedSystem(system);
}
