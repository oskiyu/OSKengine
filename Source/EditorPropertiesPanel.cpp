#include "EditorPropertiesPanel.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorPanelTitle.h"
#include "EditorUiConstants.h"


OSK::Editor::UI::PropertiesPanel::PropertiesPanel(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
	const Vector2f textSize = { size.x - 20.0f, 25.0f };

	const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
		->GetTextureView2D();

	m_title = new EditorPanelTitle(textSize);
	m_title->SetMargin(Vector2f(5.0f));
	m_title->SetPadding(Vector2f(5.0f));
	m_title->SetText("Propiedades");

	AddChild("title", m_title);

	m_subtitle = new OSK::UI::TextView(textSize);
	m_subtitle->SetMargin(Vector4f(5.0f));
	m_subtitle->SetPadding(Vector2f(5.0f));

	m_subtitle->GetSprite().SetImageView(uiView);
	m_subtitle->GetSprite().color = Constants::BackgroundAlternativeColor;

	AddChild("subtitle", m_subtitle);
}


void OSK::Editor::UI::PropertiesPanel::SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font) {
	m_font = font;

	m_title->SetFont(font);
	m_subtitle->SetFont(font);
}

void OSK::Editor::UI::PropertiesPanel::SetFontSize(USize64 fontSize) {
	m_title->SetFontSize(fontSize + 2);
	m_subtitle->SetFontSize(fontSize - 2);
}

void OSK::Editor::UI::PropertiesPanel::SetSubtitle(const std::string& text) {
	m_subtitle->SetText(text);
}

void OSK::Editor::UI::PropertiesPanel::ClearContent() {
	m_subtitle->SetInvisible();
	for (auto* view : m_content) {
		view->SetInvisible();
	}
	for (auto* view : m_textContent) {
		view->SetInvisible();
	}
}

void OSK::Editor::UI::PropertiesPanel::ShowContent() {
	m_subtitle->SetVisible();
}

void OSK::Editor::UI::PropertiesPanel::UpdateBySystem(const OSK::ECS::ISystem* system) {
	// Estado
	{
		OSK::UI::HorizontalContainer* stateContainer = nullptr;

		if (m_content.GetSize() < 1) {
			stateContainer = new OSK::UI::HorizontalContainer({ GetSize().x - 20.0f, 20.0f });

			// Nombre.
			{
				auto* name = new OSK::UI::TextView(Vector2f::Zero);
				name->SetFont(m_font);
				name->SetFontSize(14);
				name->SetMargin(Vector2f(8.0f, 2.0f));

				stateContainer->AddChild("name", name);
			}

			// Valor.
			{
				auto* value = new OSK::UI::TextView(Vector2f::Zero);
				value->SetFont(m_font);
				value->SetFontSize(14);
				value->SetMargin(Vector2f(8.0f, 2.0f));

				stateContainer->AddChild("value", value);
			}

			m_content.Insert(stateContainer);
			AddChild(std::format("content{}", m_content.GetSize()), stateContainer);
		}
		else {
			stateContainer = m_content[0];
		}

		stateContainer->SetVisible();
		stateContainer->GetChild("name")->As<OSK::UI::TextView>()->SetText("Estado");
		stateContainer->GetChild("name")->As<OSK::UI::TextView>()->AdjustSizeToText();
		stateContainer->GetChild("value")->As<OSK::UI::TextView>()->SetText(system->IsActive() ? "activo" : "inactivo");
		stateContainer->GetChild("value")->As<OSK::UI::TextView>()->AdjustSizeToText();
		stateContainer->Rebuild();
	}
	
	// Datos
	auto datos = system->SaveConfiguration();

	UIndex32 next = 1;
	for (const auto& [key, value] : datos.items()) {
		if (value.is_object()) {
			// TODO
			continue;
		}

		OSK::UI::HorizontalContainer* dataContainer = nullptr;

		if (m_content.GetSize() <= next) {
			dataContainer = new OSK::UI::HorizontalContainer({ GetSize().x - 20.0f, 20.0f });

			auto* name = new OSK::UI::TextView(Vector2f::Zero);
			name->SetFont(m_font);
			name->SetFontSize(15);
			name->SetMargin(Vector2f(8.0f, 2.0f));

			dataContainer->AddChild("name", name);

			auto* valueView = new OSK::UI::TextView(Vector2f::Zero);
			valueView->SetFont(m_font);
			valueView->SetFontSize(15);
			valueView->SetMargin(Vector2f(8.0f, 2.0f));

			dataContainer->AddChild("value", valueView);

			AddChild(std::format("content{}", m_content.GetSize()), dataContainer);
			m_content.Insert(dataContainer);
		}
		else {
			dataContainer = m_content[next];
		}

		std::string vString{};

		switch (value.type()) {
		case nlohmann::detail::value_t::boolean:
			vString = std::to_string(static_cast<bool>(value));
			break;
		case nlohmann::detail::value_t::number_float:
			vString = std::to_string(static_cast<float>(value));
			break;
		case nlohmann::detail::value_t::number_integer:
			vString = std::to_string(static_cast<int>(value));
			break;
		case nlohmann::detail::value_t::number_unsigned:
			vString = std::to_string(static_cast<unsigned int>(value));
			break;
		case nlohmann::detail::value_t::string:
			vString = static_cast<std::string>(value);
			break;
		case nlohmann::detail::value_t::null:
			vString = "null";
			break;
		}

		dataContainer->SetVisible();
		dataContainer->GetChild("name")->As<OSK::UI::TextView>()->SetText(key);
		dataContainer->GetChild("name")->As<OSK::UI::TextView>()->AdjustSizeToText();
		dataContainer->GetChild("value")->As<OSK::UI::TextView>()->SetText(vString);
		dataContainer->GetChild("value")->As<OSK::UI::TextView>()->AdjustSizeToText();
		dataContainer->AdjustSizeToChildren();
		dataContainer->Rebuild();

		next++;
	}

	for (; next < m_content.GetSize(); next++) {
		m_content[next]->SetInvisible();
	}
}

void OSK::Editor::UI::PropertiesPanel::UpdateByObject(const OSK::ECS::GameObjectIndex obj) {
	const auto componentTypes = Engine::GetEcs()->GetObjectComponentsTypes(obj);

	UIndex32 next = 0;

	for (const OSK::ECS::ComponentType componentType : componentTypes) {
		const std::string name = Engine::GetEcs()->GetComponentTypeName(componentType);

		OSK::UI::TextView* componentNameView = nullptr;

		if (m_textContent.GetSize() <= next) {
			componentNameView = new OSK::UI::TextView({ GetSize().x - 20.0f, 20.0f });

			componentNameView->SetFont(m_font);
			componentNameView->SetFontSize(15);
			componentNameView->SetMargin(Vector2f(8.0f, 2.0f));

			AddChild(std::format("content{}", m_textContent.GetSize()), componentNameView);
			m_textContent.Insert(componentNameView);
		}
		else {
			componentNameView = m_textContent[next];
		}

		componentNameView->SetVisible();
		componentNameView->SetText(name);
		componentNameView->AdjustSizeToText();

		next++;
	}

	for (; next < m_textContent.GetSize(); next++) {
		m_textContent[next]->SetInvisible();
	}
}
