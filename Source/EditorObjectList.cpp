#include "EditorObjectList.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorPropertiesPanel.h"

#include "EditorUiConstants.h"
#include "EditorPanelTitle.h"


OSK::Editor::UI::ObjectList::ObjectList(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
	const Vector2f textSize = { size.x - 20.0f, 25.0f };

	const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
		->GetTextureView2D();

	SetMargin(Vector2f::Zero);

	GetSprite().SetImageView(uiView);
	GetSprite().color = Constants::BackgroundColor;

	m_title = new EditorPanelTitle(textSize);
	m_title->SetMargin(Vector2f(5.0f));
	m_title->SetPadding(Vector2f(5.0f));
	m_title->SetText("Objetos");

	AddChild("title", m_title);


	for (UIndex64 i = 0; i < MaxShownObjects; i++) {
		auto* view = new OSK::UI::Button(textSize);
		view->SetPadding(Vector2f(5.0f));
		view->SetMargin(Vector4f(5.0f, 1.0f, 5.0f, 1.0f));
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

				auto text = static_cast<std::string>(view->GetText().substr(view->GetText().find_first_of("0123456789")));
				auto obj = std::stoull(text);
				m_propertiesPanel->UpdateByObject(OSK::ECS::GameObjectIndex(obj));
			}
			else {
				m_propertiesPanel->ClearContent();
			}
			});
	}

	m_propertiesPanel = new PropertiesPanel({ size.x, 300.0f });
	m_propertiesPanel->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::CENTER_Y);
	AddChild(PropertiesPanel::Name, m_propertiesPanel);
}


void OSK::Editor::UI::ObjectList::ClearObjects() {
	for (OSK::UI::Button* view : m_textViews) {
		view->SetInvisible();
	}
}

void OSK::Editor::UI::ObjectList::SetObjects(std::span<const OSK::ECS::GameObjectIndex> objects) {
	for (UIndex64 i = 0; i < std::min(objects.size(), m_textViews.GetSize()); i++) {
		m_textViews[i]->SetVisible();
		m_textViews[i]->SetText(std::format("Objeto {}", objects[i].Get()));
	}
}

void OSK::Editor::UI::ObjectList::SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font) {
	m_title->SetFont(font);
	m_propertiesPanel->SetFont(font);

	for (OSK::UI::Button* view : m_textViews) {
		view->SetTextFont(font);
	}
}

void OSK::Editor::UI::ObjectList::SetFontSize(USize64 fontSize) {
	m_title->SetFontSize(fontSize + 2);

	m_propertiesPanel->SetFontSize(fontSize + 2);

	for (OSK::UI::Button* view : m_textViews) {
		view->SetTextFontSize(fontSize);
	}
}

void OSK::Editor::UI::ObjectList::ClearSelection() {
	for (auto* view : m_textViews) {
		if (!view->IsVisible()) {
			break;
		}

		view->SetState(OSK::UI::Button::State::DEFAULT);
	}
}
