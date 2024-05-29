#include "EditorBottomBar.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"
#include "Texture.h"

#include "UiImageView.h"
#include "UiTextView.h"

#include "EditorUiConstants.h"

// module OSKengine.Editor.Ui:BottomBar;

OSK::Editor::UI::EditorBottomBar::EditorBottomBar(const Vector2f& size) : OSK::UI::HorizontalContainer({ size.x, 22.0f }) {
	auto editorFont = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");
	const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
		->GetTextureView2D();

	SetMargin(Vector2f::Zero);

	auto iconTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json");
	const auto iconView = &iconTexture->GetTextureView2D();

	GetSprite().SetImageView(uiView);
	GetSprite().color = Constants::DarkColor;

	OSK::UI::ImageView* uiIcon = new OSK::UI::ImageView(iconTexture->GetSize().ToVector2f() * (16.0f / iconTexture->GetSize().ToVector2f().y));
	uiIcon->GetSprite().SetImageView(iconView);
	uiIcon->SetAnchor(OSK::UI::Anchor::FULLY_CENTERED);
	uiIcon->SetMargin(Vector2f(5.0f, 0.0f));

	AddChild("icon", uiIcon);

	auto* versionView = new OSK::UI::TextView({ 20.0f, 15.0f });
	versionView->SetFont(editorFont);
	versionView->SetFontSize(14);
	versionView->SetText(static_cast<std::string>(Engine::GetBuild()));
	versionView->AdjustSizeToText();
	versionView->SetMargin(Vector2f(10.0f, 0.0f));

	AddChild("version", versionView);

	auto* numThreadsView = new OSK::UI::TextView({ 20.0f, 15.0f });
	numThreadsView->SetFont(editorFont);
	numThreadsView->SetFontSize(14);
	numThreadsView->SetText(std::format("{} hilos", Engine::GetJobSystem()->GetNumThreads()));
	numThreadsView->AdjustSizeToText();
	numThreadsView->SetMargin(Vector2f(10.0f, 0.0f));

	AddChild("threads", numThreadsView);

	auto* gpuView = new OSK::UI::TextView({ 20.0f, 15.0f });
	gpuView->SetFont(editorFont);
	gpuView->SetFontSize(14);
	gpuView->SetText(std::format("{}", Engine::GetRenderer()->GetGpu()->GetName()));
	gpuView->AdjustSizeToText();
	gpuView->SetMargin(Vector2f(10.0f, 0.0f));

	AddChild("gpu", gpuView);

	m_fpsView = new OSK::UI::TextView({ 50.0f, 15.0f });
	m_fpsView->SetFont(editorFont);
	m_fpsView->SetFontSize(14);
	m_fpsView->SetMargin(Vector2f(10.0f, 0.0f));
	m_fpsView->SetText("XXX fps");
	m_fpsView->AdjustSizeToText();

	AddChild("fps", m_fpsView);
}

void OSK::Editor::UI::EditorBottomBar::Update(TDeltaTime deltaTime) {
	m_accumulatedTime += deltaTime;
	m_frameCount++;

	if (m_accumulatedTime > 1.0f) {
		m_fpsView->SetText(std::format("{:.2f} fps", m_frameCount / m_accumulatedTime));
		m_accumulatedTime = 0.0f;
		m_frameCount = 0;
	}
}
