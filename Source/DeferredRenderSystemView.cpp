#include "DeferredRenderSystemView.h"

#include "OSKengine.h"
#include "UiTextView.h"
#include "EditorUiConstants.h"
#include "UiCollapsibleWrapper.h"
#include "SdfDrawCall2D.h"
#include "SdfDrawCallType2D.h"

OSK::Editor::Views::DeferredRenderSystemView::DeferredRenderSystemView(const Vector2f& size) : ISystemView(size, (std::string)GetSystemName()),
	m_irradianceMapView(new OSK::UI::TextView(size)),
	m_specularMapView(new OSK::UI::TextView(size)),
	m_cameraObjectView(new OSK::UI::TextView(size)),
	m_resolvePassView(new OSK::UI::TextView(size)),
	m_originalTextSize({ 220.0f, 14.0f })
{
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_irradianceMapView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_irradianceMapView->SetFont(font);
	m_irradianceMapView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_specularMapView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_specularMapView->SetFont(font);
	m_specularMapView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_cameraObjectView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_cameraObjectView->SetFont(font);
	m_cameraObjectView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	m_shaderPasses = new OSK::UI::CollapsibleWrapper(
		new VerticalContainer({ 320.0f, 16.0f }),
		"> Pases de renderizado (GBuffer):", 
		font, 
		UI::Constants::SecondaryFontSize);
	m_shaderPasses->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	ConfigureCollapsible(m_shaderPasses);

	m_shadowsPasses = new OSK::UI::CollapsibleWrapper(
		new VerticalContainer({ 320.0f, 16.0f }),
		"> Pases de renderizado (sombras):", 
		font, 
		UI::Constants::SecondaryFontSize);
	m_shadowsPasses->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	ConfigureCollapsible(m_shadowsPasses);

	m_resolvePassView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_resolvePassView->SetFont(font);
	m_resolvePassView->SetFontSize(Editor::UI::Constants::SecondaryFontSize);

	AddChild("m_irradianceMapView", m_irradianceMapView);
	AddChild("m_specularMapView", m_specularMapView);
	AddChild("m_cameraObjectView", m_cameraObjectView);
	AddChild("m_shaderPasses", m_shaderPasses);
	AddChild("m_shadowsPasses", m_shadowsPasses);
	AddChild("m_resolvePassView", m_resolvePassView);

	AdjustSizeToChildren();
}

void OSK::Editor::Views::DeferredRenderSystemView::SetIrradianceMapName(std::string_view name) {
	m_irradianceMapView->SetText(std::format("Irradiancia IBL: {}", name));
}

void OSK::Editor::Views::DeferredRenderSystemView::SetSpecularMapName(std::string_view name) {
	m_specularMapView->SetText(std::format("Especular IBL: {}", name));
}

void OSK::Editor::Views::DeferredRenderSystemView::SetCameraObject(ECS::GameObjectIndex object) {
	m_cameraObjectView->SetText(std::format("ID de la cámara: {}", object.Get()));
}

void OSK::Editor::Views::DeferredRenderSystemView::SetResolverPassName(std::string_view name) {
	m_resolvePassView->SetText(std::format("Pase de resolución: {}", name));
}

void OSK::Editor::Views::DeferredRenderSystemView::AddShaderPass(std::string_view name) {
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

	m_shaderPasses->GetContent()->As<VerticalContainer>()->AddChild(static_cast<std::string>(view->GetText()), view);
	m_shaderPasses->GetContent()->As<VerticalContainer>()->AdjustSizeToChildren();
	m_shaderPasses->AdjustSizeToChildren();
	m_shaderPasses->Rebuild();

	m_shaderPassesViews.Insert(view);

	Rebuild();
}

void OSK::Editor::Views::DeferredRenderSystemView::AddShadowsPass(std::string_view name) {
	for (const auto& pass : m_shadowsPassesViews) {
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

	m_shadowsPasses->GetContent()->As<VerticalContainer>()->AddChild(static_cast<std::string>(view->GetText()), view);
	m_shadowsPasses->GetContent()->As<VerticalContainer>()->AdjustSizeToChildren();
	m_shadowsPasses->AdjustSizeToChildren();
	m_shadowsPasses->Rebuild();

	m_shadowsPassesViews.Insert(view);

	Rebuild();
}

std::string OSK::Editor::Views::DeferredRenderSystemView::GetPassDisplayName(std::string_view name) {
	return std::format("{}", name);
}

void OSK::Editor::Views::DeferredRenderSystemView::ConfigureCollapsible(OSK::UI::CollapsibleWrapper* collapsible) const {
	collapsible->GetToggleBar()->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	collapsible->GetToggleBar()->SetTextAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	// collapsible->SetMargin(Vector2f::Zero);
	// collapsible->SetPadding(Vector2f::Zero);
	collapsible->GetToggleBar()->SetMargin(Vector2f::Zero);
	collapsible->GetToggleBar()->SetPadding(Vector2f::Zero);
	collapsible->AdjustToggleBarSize();

	OSK::GRAPHICS::SdfDrawCall2D background{};
	background.contentType = OSK::GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
	background.fill = true;
	background.mainColor = UI::Constants::BackgroundAlternativeColor;
	background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
	background.transform.SetPosition(GetContentTopLeftPosition());
	background.transform.SetScale(GetSize());

	// Botón.
	{
		GRAPHICS::SdfDrawCall2D background{};
		background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		background.fill = true;
		background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		background.transform.SetPosition(GetContentTopLeftPosition());
		background.transform.SetScale(GetSize());

		background.mainColor = Editor::UI::Constants::DarkColor;
		// collapsible->GetToggleBar()->GetDefaultDrawCalls().Insert(background);

		background.mainColor = Editor::UI::Constants::HoveredColor;
		// collapsible->GetToggleBar()->GetSelectedDrawCalls().Insert(background);

		background.mainColor = Editor::UI::Constants::DarkColor;
		// collapsible->GetToggleBar()->GetPressedDrawCalls().Insert(background);
	}

	// collapsible->AddDrawCall(background);
	collapsible->Expand();
}
