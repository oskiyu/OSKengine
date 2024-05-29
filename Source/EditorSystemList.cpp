#include "EditorSystemList.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorUiConstants.h"
#include "EditorPanelTitle.h"
#include "EditorPropertiesPanel.h"


OSK::Editor::UI::SystemList::SystemList(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
	const Vector2f textSize = { size.x - 20.0f, 25.0f };

	const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
		->GetTextureView2D();

	SetMargin(Vector2f::Zero);

	GetSprite().SetImageView(uiView);
	GetSprite().color = Constants::BackgroundColor;

	m_title = new EditorPanelTitle(textSize);
	m_title->SetMargin(Vector2f(5.0f));
	m_title->SetPadding(Vector2f(5.0f));
	m_title->SetText("Sistemas");

	AddChild("title", m_title);

	m_propertiesPanel = new PropertiesPanel({ size.x, 300.0f });
	m_propertiesPanel->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::CENTER_Y);

	for (UIndex64 i = 0; i < MaxShownSystems; i++) {
		auto* view = new OSK::UI::Button(textSize);
		view->SetMargin(Vector4f(5.0f, 1.0f, 5.0f, 1.0f));
		view->SetPadding(Vector2f(5.0f));
		view->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::CENTER_Y);

		view->GetSelectedSprite().SetImageView(uiView);
		view->GetSelectedSprite().color = Constants::HoveredColor;

		view->GetPressedSprite().SetImageView(uiView);
		view->GetPressedSprite().color = Constants::SelectedColor;

		if (i % 2 == 1) {
			view->GetDefaultSprite().SetImageView(uiView);
			view->GetDefaultSprite().color = Constants::BackgroundAlternativeColor;
		}

		AddChild(std::to_string(i), view);
		m_textViews.Insert(view);
	}

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

	AdjustSizeToChildren();
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
