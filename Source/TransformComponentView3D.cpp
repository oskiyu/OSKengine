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

	GetUnderlyingContainer()->AddChild("m_positionView", UniquePtr<OSK::UI::IElement>(m_positionView));
	GetUnderlyingContainer()->AddChild("m_rotationView", UniquePtr<OSK::UI::IElement>(m_rotationView));
	GetUnderlyingContainer()->AddChild("m_sizeView", UniquePtr<OSK::UI::IElement>(m_sizeView));

	SetPosition(Vector3f::Zero);
	SetRotation(Vector3f::Zero);
	SetScale(Vector3f::Zero);

	GetUnderlyingContainer()->AdjustSizeToChildren();
}

void OSK::Editor::Views::TransformComponentView3D::SetPosition(const Vector3f& position) {
	m_positionView->SetText(std::format("Posici�n: X: {:.1f} Y: {:.1f} Z: {:.1f} m", position.x, position.y, position.z));
}

void OSK::Editor::Views::TransformComponentView3D::SetRotation(const Vector3f& rotation) {
	m_rotationView->SetText(std::format("Rotaci�n: X: {:.1f} Y: {:.1f} Z: {:.1f} �", rotation.x, rotation.y, rotation.z));
}

void OSK::Editor::Views::TransformComponentView3D::SetScale(const Vector3f& scale) {
	m_sizeView->SetText(std::format("Escala: X: {:.1f} Y: {:.1f} Z: {:.1f} %", scale.x, scale.y, scale.z));
}
