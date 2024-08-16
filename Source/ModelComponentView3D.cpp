#include "ModelComponentView3D.h"

#include "UiTextView.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"
#include "FontInstance.h"

#include "EditorUiConstants.h"

OSK::Editor::Views::ModelComponentView3D::ModelComponentView3D(const Vector2f& size) : IComponentView(size, "3D Model"),
	m_assetPathView(new OSK::UI::TextView(size)),
	m_castsShadowsView(new OSK::UI::TextView(size)),
	m_isAnimatedView(new OSK::UI::TextView(size)),
	m_shaderPassesTitle(new OSK::UI::TextView(size)), 
	m_originalTextSize(size)
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_assetPathView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_assetPathView->SetFont(font);
	m_assetPathView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_castsShadowsView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_castsShadowsView->SetFont(font);
	m_castsShadowsView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_isAnimatedView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_isAnimatedView->SetFont(font);
	m_isAnimatedView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_shaderPassesTitle->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_shaderPassesTitle->SetFont(font);
	m_shaderPassesTitle->SetFontSize(Editor::UI::Constants::SecondaryFontSize);
	m_shaderPassesTitle->SetText("Pases de renderizado:");

	GetUnderlyingContainer()->AddChild("m_assetPathView", m_assetPathView);
	GetUnderlyingContainer()->AddChild("m_castsShadowsView", m_castsShadowsView);
	GetUnderlyingContainer()->AddChild("m_isAnimatedView", m_isAnimatedView);
	GetUnderlyingContainer()->AddChild("m_shaderPassesTitle", m_shaderPassesTitle);

	GetUnderlyingContainer()->AdjustSizeToChildren();
}

void OSK::Editor::Views::ModelComponentView3D::SetAssetPath(const std::string& path) {
	m_assetPathView->SetText(std::format("Asset: {}", path));
}

void OSK::Editor::Views::ModelComponentView3D::SetCastsShadows(bool castsShadows) {
	m_castsShadowsView->SetText(std::format("Produce sombras: {}", castsShadows ? "sí" : "no"));
}

void OSK::Editor::Views::ModelComponentView3D::SetIsAnimated(bool isAnimated) {
	m_isAnimatedView->SetText(std::format("Modelo animado: {}", isAnimated ? "sí" : "no"));
}

void OSK::Editor::Views::ModelComponentView3D::ClearShaderPasses() {
	/* for (auto& pass : m_shaderPassesViews) {
		DeleteChild(pass->GetText());
	}

	m_shaderPassesViews.Empty(); */
}

void OSK::Editor::Views::ModelComponentView3D::AddShaderPass(const std::string& name) {
	for (const auto& pass : m_shaderPassesViews) {
		if (pass->GetText() == GetPassDisplayName(name)) {
			return;
		}
	}

	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	OSK::UI::TextView* view = new OSK::UI::TextView(m_originalTextSize);

	view->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	view->SetFont(font);
	view->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	view->SetText(GetPassDisplayName(name));

	GetUnderlyingContainer()->AddChild(static_cast<std::string>(view->GetText()), view);
	GetUnderlyingContainer()->AdjustSizeToChildren();
	AdjustSizeToChildren();

	m_shaderPassesViews.Insert(view);
}

void OSK::Editor::Views::ModelComponentView3D::DeleteUnusedPasses() {

}

std::string OSK::Editor::Views::ModelComponentView3D::GetPassDisplayName(const std::string& name) {
	return std::format("\t{}", name);
}
