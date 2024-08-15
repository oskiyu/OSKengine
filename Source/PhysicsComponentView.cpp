#include "PhysicsComponentView.h"

#include "UiTextView.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"

#include "EditorUiConstants.h"

OSK::Editor::Views::PhysicsComponentView::PhysicsComponentView(const Vector2f& size) : IComponentView(size, "Physics"),
	m_massView(new OSK::UI::TextView(size)),
	m_velocityView(new OSK::UI::TextView(size)), 
	m_accelerationView(new OSK::UI::TextView(size))
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_massView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_massView->SetFont(font);
	m_massView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_velocityView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_velocityView->SetFont(font);
	m_velocityView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_accelerationView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_accelerationView->SetFont(font);
	m_accelerationView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_massView", m_massView);
	AddChild("m_velocityView", m_velocityView);
	AddChild("m_accelerationView", m_accelerationView);

	AdjustSizeToChildren();
}

void OSK::Editor::Views::PhysicsComponentView::SetMass(float value) {
	m_massView->SetText(std::format("Masa: {:.2f}kg", value));
}

void OSK::Editor::Views::PhysicsComponentView::SetImmobile() {
	m_massView->SetText("Objeto inmóvil");
}

void OSK::Editor::Views::PhysicsComponentView::SetVelocity(const Vector3f& velocity) {
	m_velocityView->SetText(std::format("Velocidad: {:.1f} {:.1f} {:.1f} m/s", velocity.x, velocity.y, velocity.z));
}

void OSK::Editor::Views::PhysicsComponentView::SetAcceleration(const Vector3f& acceleration) {
	m_accelerationView->SetText(std::format("Acceleration: {:.1f} {:.1f} {:.1f} m/s", acceleration.x, acceleration.y, acceleration.z));
}
