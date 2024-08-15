#include "CameraComponentView.h"

#include "UiTextView.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"

#include "EditorUiConstants.h"

OSK::Editor::Views::CameraComponentView::CameraComponentView(const Vector2f& size) : IComponentView(size, "Camera"), m_fovView(new OSK::UI::TextView(size)) {
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>(Editor::UI::Constants::EditorFontPath);

	m_fovView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_fovView->SetFont(font);
	m_fovView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("fov", m_fovView);

	AdjustSizeToChildren();
}

OSK::UI::TextView* OSK::Editor::Views::CameraComponentView::GetFovView() {
	return m_fovView;
}
