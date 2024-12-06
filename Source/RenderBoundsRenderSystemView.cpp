#include "RenderBoundsRenderSystemView.h"

#include "OSKengine.h"
#include "UiTextView.h"
#include "EditorUiConstants.h"
#include "UiCollapsibleWrapper.h"
#include "SdfDrawCall2D.h"
#include "SdfDrawCallType2D.h"

OSK::Editor::Views::RenderBoundsRenderSystemView::RenderBoundsRenderSystemView(const Vector2f& size) : ISystemView(size, (std::string)GetSystemName()),
	m_cameraObjectView(new OSK::UI::TextView(size)),
	m_materialNameView(new OSK::UI::TextView(size))
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_cameraObjectView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_cameraObjectView->SetFont(font);
	m_cameraObjectView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_materialNameView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_materialNameView->SetFont(font);
	m_materialNameView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_cameraObjectView", m_cameraObjectView);
	AddChild("m_materialNameView", m_materialNameView);

	AdjustSizeToChildren();
}

void OSK::Editor::Views::RenderBoundsRenderSystemView::SetCameraObject(ECS::GameObjectIndex object) {
	m_cameraObjectView->SetText(std::format("ID de la cámara: {}", object.Get()));
}

void OSK::Editor::Views::RenderBoundsRenderSystemView::SetMaterialName(std::string_view name) {
	m_materialNameView->SetText(std::format("Material: {}", name));
}
