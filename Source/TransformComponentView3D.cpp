#include "TransformComponentView3D.h"

#include "UiTextView.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"

#include "EditorUiConstants.h"

OSK::Editor::Views::TransformComponentView3D::TransformComponentView3D(const Vector2f& size) : IComponentView(size, "Transform 3D"),
	m_positionView(new OSK::UI::TextView(size)),
	m_rotationView(new OSK::UI::TextView(size)),
	m_sizeView(new OSK::UI::TextView(size))
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_positionView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_positionView->SetFont(font);
	m_positionView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_rotationView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_rotationView->SetFont(font);
	m_rotationView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_sizeView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_sizeView->SetFont(font);
	m_sizeView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_positionView", m_positionView);
	AddChild("m_rotationView", m_rotationView);
	AddChild("m_sizeView", m_sizeView);

	SetPosition(Vector3f::Zero);
	SetRotation(Vector3f::Zero);
	SetScale(Vector3f::Zero);

	AdjustSizeToChildren();
}

void OSK::Editor::Views::TransformComponentView3D::SetPosition(const Vector3f& position) {
	m_positionView->SetText(std::format("Posición: {:.1f} {:.1f} {:.1f} m", position.x, position.y, position.z));
}

void OSK::Editor::Views::TransformComponentView3D::SetRotation(const Vector3f& rotation) {
	m_rotationView->SetText(std::format("Rotación: {:.1f} {:.1f} {:.1f} º", rotation.x, rotation.y, rotation.z));
}

void OSK::Editor::Views::TransformComponentView3D::SetScale(const Vector3f& scale) {
	m_sizeView->SetText(std::format("Escala: {:.1f} {:.1f} {:.1f} %", scale.x, scale.y, scale.z));
}
