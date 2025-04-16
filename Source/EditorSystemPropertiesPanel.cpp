#include "EditorSystemPropertiesPanel.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "AssetManager.h"
#include "EditorUiConstants.h"

OSK::Editor::UI::SystemPropertiesPanel::SystemPropertiesPanel(const Vector2f& size) : PropertiesPanel(size) {
	AddStateObjectButton();
}

void OSK::Editor::UI::SystemPropertiesPanel::SetView(UniquePtr<OSK::Editor::Views::ISystemView>&& view) {
	if (HasChild(ChildName)) {
		DeleteChild(ChildName);
	}

	AddChild(ChildName, std::move(view));
}

void OSK::Editor::UI::SystemPropertiesPanel::ClearView() {
	if (HasChild(ChildName)) {
		DeleteChild(ChildName);
	}
}

void OSK::Editor::UI::SystemPropertiesPanel::ClearContent() {
	PropertiesPanel::ClearContent();
	ClearView();
}

void OSK::Editor::UI::SystemPropertiesPanel::SetSelectedSystem(ECS::ISystem* system) {
	m_currentlySelectedSystem = system;

	if (system) {
		m_stateButton->Unlock();
		m_stateButton->SetText(m_currentlySelectedSystem->IsActive() ? "Activado" : "Desactivado");
		m_stateButton->SetState(m_currentlySelectedSystem->IsActive() ? OSK::UI::Button::State::PRESSED : OSK::UI::Button::State::DEFAULT);
	}
	else {
		m_stateButton->Lock();
		m_stateButton->SetText("Ningún sistema seleccionado");
		m_stateButton->SetState(OSK::UI::Button::State::DEFAULT);
	}
}

void OSK::Editor::UI::SystemPropertiesPanel::AddStateObjectButton() {
	m_stateButton = new OSK::UI::Button({ GetSize().x - 20.0f, 25.0f }, "Inactivo");

	m_stateButton->SetMargin({
		m_stateButton->GetMarging2D().x,
		m_stateButton->GetMarging2D().y + 3.0f
		});

	m_stateButton->SetType(OSK::UI::Button::Type::TOGGLE);
	m_stateButton->SetTextFont(Engine::GetAssetManager()->Load<OSK::ASSETS::Font>(OSK::Editor::UI::Constants::EditorFontPath));
	m_stateButton->SetTextFontSize(OSK::Editor::UI::Constants::SecondaryFontSize);
	m_stateButton->SetCallback([this](bool state) {
		m_stateButton->SetText(state ? "Activo" : "Inactivo");

		if (m_currentlySelectedSystem) {
			m_currentlySelectedSystem->SetActivationStatus(state);
		}
		});

	{
		GRAPHICS::SdfDrawCall2D background{};
		background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		background.fill = true;
		background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		background.transform.SetPosition(GetContentTopLeftPosition());
		background.transform.SetScale(m_stateButton->GetSize());

		background.mainColor = OSK::Editor::UI::Constants::DefaultRedColor;
		m_stateButton->GetDefaultDrawCalls().Insert(background);

		background.mainColor = OSK::Editor::UI::Constants::HoveredColor;
		m_stateButton->GetSelectedDrawCalls().Insert(background);

		background.mainColor = OSK::Editor::UI::Constants::DefaultGreenColor;
		m_stateButton->GetPressedDrawCalls().Insert(background);
	}

	AddChild("stateButton", UniquePtr(m_stateButton));
}
