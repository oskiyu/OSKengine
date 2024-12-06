#include "ColliderRenderSystemView.h"

#include "OSKengine.h"
#include "UiTextView.h"
#include "EditorUiConstants.h"
#include "UiCollapsibleWrapper.h"
#include "SdfDrawCall2D.h"
#include "SdfDrawCallType2D.h"

OSK::Editor::Views::ColliderRenderSystemView::ColliderRenderSystemView(const Vector2f& size) : ISystemView(size, (std::string)GetSystemName()),
	m_cameraObjectView(new OSK::UI::TextView(size)),
	m_topLevelMaterialNameView(new OSK::UI::TextView(size)),
	m_bottomLevelMaterialNameView(new OSK::UI::TextView(size)),
	m_pointsMaterialNameView(new OSK::UI::TextView(size))
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_cameraObjectView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_cameraObjectView->SetFont(font);
	m_cameraObjectView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_topLevelMaterialNameView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_topLevelMaterialNameView->SetFont(font);
	m_topLevelMaterialNameView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_bottomLevelMaterialNameView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_bottomLevelMaterialNameView->SetFont(font);
	m_bottomLevelMaterialNameView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_pointsMaterialNameView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_pointsMaterialNameView->SetFont(font);
	m_pointsMaterialNameView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_cameraObjectView", m_cameraObjectView);
	AddChild("m_topLevelMaterialNameView", m_topLevelMaterialNameView);
	AddChild("m_bottomLevelMaterialNameView", m_bottomLevelMaterialNameView);
	AddChild("m_pointsMaterialNameView", m_pointsMaterialNameView);

	AdjustSizeToChildren();
}

void OSK::Editor::Views::ColliderRenderSystemView::SetCameraObject(ECS::GameObjectIndex object) {
	m_cameraObjectView->SetText(std::format("ID de la cámara: {}", object.Get()));
}

void OSK::Editor::Views::ColliderRenderSystemView::SetTopLevelMaterialName(std::string_view name) {
	m_topLevelMaterialNameView->SetText(std::format("Material de alto nivel: {}", name));
}

void OSK::Editor::Views::ColliderRenderSystemView::SetBottomLevelMaterialName(std::string_view name) {
	m_bottomLevelMaterialNameView->SetText(std::format("Material de bajo nivel: {}", name));
}

void OSK::Editor::Views::ColliderRenderSystemView::SetPointsMaterialName(std::string_view name) {
	m_pointsMaterialNameView->SetText(std::format("Material de puntos: {}", name));
}
