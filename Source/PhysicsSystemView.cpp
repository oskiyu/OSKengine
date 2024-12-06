#include "PhysicsSystemView.h"

#include "OSKengine.h"
#include "UiTextView.h"
#include "EditorUiConstants.h"

OSK::Editor::Views::PhysicsSystemView::PhysicsSystemView(const Vector2f& size) : ISystemView(size, (std::string)GetSystemName()), m_gravityView(new OSK::UI::TextView(size)) {
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_gravityView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_gravityView->SetFont(font);
	m_gravityView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_gravityView", m_gravityView);

	AdjustSizeToChildren();
}

void OSK::Editor::Views::PhysicsSystemView::SetGravity(const Vector3f& gravity) {
	m_gravityView->SetText(std::format("Gravedad: X: {:.1f} Y: {:.1f} Z: {:.1f} m/s2", gravity.x, gravity.y, gravity.z));
}
