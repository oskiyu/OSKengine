#include "EditorSystemPropertiesPanel.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "AssetManager.h"
#include "EditorUiConstants.h"

OSK::Editor::UI::SystemPropertiesPanel::SystemPropertiesPanel(const Vector2f& size) : PropertiesPanel(size) {

}

void OSK::Editor::UI::SystemPropertiesPanel::UpdateBySystem(const OSK::ECS::ISystem* system) {
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>(Editor::UI::Constants::EditorFontPath);

	// Estado
	{
		OSK::UI::HorizontalContainer* stateContainer = nullptr;

		if (m_content.GetSize() < 1) {
			stateContainer = new OSK::UI::HorizontalContainer({ GetSize().x - 20.0f, 20.0f });

			// Nombre.
			{
				auto* name = new OSK::UI::TextView(Vector2f::Zero);
				name->SetFont(font);
				name->SetFontSize(14);
				name->SetMargin(Vector2f(8.0f, 2.0f));

				stateContainer->AddChild("name", name);
			}

			// Valor.
			{
				auto* value = new OSK::UI::TextView(Vector2f::Zero);
				value->SetFont(font);
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
			name->SetFont(font);
			name->SetFontSize(15);
			name->SetMargin(Vector2f(8.0f, 2.0f));

			dataContainer->AddChild("name", name);

			auto* valueView = new OSK::UI::TextView(Vector2f::Zero);
			valueView->SetFont(font);
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
