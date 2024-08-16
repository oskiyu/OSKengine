#include "EditorSystemList.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorUiConstants.h"
#include "EditorPanelTitle.h"
#include "EditorSystemPropertiesPanel.h"


OSK::Editor::UI::SystemList::SystemList(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
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
		m_title->SetMargin(Vector2f(5.0f));
		m_title->SetPadding(Vector2f(5.0f));
		m_title->SetText("Sistemas");

		AddChild("title", m_title);
	}

	// Botones de sistemas.
	for (UIndex64 i = 0; i < MaxShownSystems; i++) {
		auto* view = new OSK::UI::Button(textSize);
		view->SetMargin(Vector4f(5.0f, 1.0f, 5.0f, 1.0f));
		view->SetPadding(Vector2f(5.0f));

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

		if (i % 2 == 1) {
			GRAPHICS::SdfDrawCall2D defaultDrawCall{};
			defaultDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			defaultDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			defaultDrawCall.mainColor = Constants::BackgroundAlternativeColor;
			defaultDrawCall.transform.SetScale(view->GetSize());

			view->GetDefaultDrawCalls().Insert(defaultDrawCall);
		}

		AddChild(std::to_string(i), view);
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
					m_propertiesPanel->ShowContent();
					m_propertiesPanel->SetSubtitle(static_cast<std::string>(view->GetText()));
					m_propertiesPanel->UpdateBySystem(OSK::Engine::GetEcs()->GetSystemByName(view->GetText()));
				}
				else {
					m_propertiesPanel->ClearContent();
				}
				});
		}

		AddChild(PropertiesPanel::Name, m_propertiesPanel);
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

void OSK::Editor::UI::SystemList::SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font) {
	m_title->SetFont(font);
	m_propertiesPanel->SetFont(font);

	for (OSK::UI::Button* view : m_textViews) {
		view->SetTextFont(font);
	}
}

void OSK::Editor::UI::SystemList::SetFontSize(USize64 fontSize) {
	m_title->SetFontSize(fontSize + 2);
	m_propertiesPanel->SetFontSize(fontSize + 2);
	
	for (OSK::UI::Button* view : m_textViews) {
		view->SetTextFontSize(fontSize);
	}
}

void OSK::Editor::UI::SystemList::ClearSelection() {
	for (auto* view : m_textViews) {
		if (!view->IsVisible()) {
			break;
		}

		view->SetState(OSK::UI::Button::State::DEFAULT);
	}
}
