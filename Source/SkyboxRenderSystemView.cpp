#include "SkyboxRenderSystemView.h"

#include "OSKengine.h"
#include "UiTextView.h"
#include "EditorUiConstants.h"

OSK::Editor::Views::SkyboxRenderSystemView::SkyboxRenderSystemView(const Vector2f& size) : ISystemView(size, (std::string)GetSystemName()), 
	m_textureNameView(new OSK::UI::TextView(size)),
	m_materialNameView(new OSK::UI::TextView(size)),
	m_cameraObjectView(new OSK::UI::TextView(size))
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_textureNameView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_textureNameView->SetFont(font);
	m_textureNameView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_materialNameView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_materialNameView->SetFont(font);
	m_materialNameView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_cameraObjectView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_cameraObjectView->SetFont(font);
	m_cameraObjectView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_textureNameView", UniquePtr<OSK::UI::IElement>(m_textureNameView));
	AddChild("m_materialNameView", UniquePtr<OSK::UI::IElement>(m_materialNameView));
	AddChild("m_cameraObjectView", UniquePtr<OSK::UI::IElement>(m_cameraObjectView));

	AdjustSizeToChildren();
}

void OSK::Editor::Views::SkyboxRenderSystemView::SetTextureName(const std::string& name) {
	m_textureNameView->SetText(std::format("Textura: {}", name));
}

void OSK::Editor::Views::SkyboxRenderSystemView::SetMaterialName(const std::string& name) {
	m_materialNameView->SetText(std::format("Material: {}", name));
}

void OSK::Editor::Views::SkyboxRenderSystemView::SetCameraObject(ECS::GameObjectIndex object) {
	m_cameraObjectView->SetText(std::format("ID de la cámara: {}", object.Get()));
}
