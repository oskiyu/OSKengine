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

	SetMargin(Vector2f::Zero);

	auto iconTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json");
	const auto iconView = &iconTexture->GetTextureView2D();

	// Renderizado de la barra.
	{
		GRAPHICS::SdfDrawCall2D barDrawCall{};
		barDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		barDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		barDrawCall.mainColor = Constants::DarkColor;
		barDrawCall.transform.SetScale(GetSize());

		m_barDrawCallIndex = GetAllDrawCalls().GetSize();

		AddDrawCall(barDrawCall);
	}
	
	// Renderizado del icono.
	{
		GRAPHICS::SdfDrawCall2D iconDrawCall{};
		iconDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::TEXTURE;
		iconDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		iconDrawCall.mainColor = Color::White;
		iconDrawCall.texture = iconView;
		iconDrawCall.textureCoordinates = GRAPHICS::TextureCoordinates2D::Pixels({
			0.0f, 0.0f,
			iconView->GetSize2D().ToVector2f().x,
			iconView->GetSize2D().ToVector2f().y });

		OSK::UI::ImageView* uiIcon = new OSK::UI::ImageView(iconTexture->GetSize().ToVector2f() * (16.0f / iconTexture->GetSize().ToVector2f().y));
		iconDrawCall.transform.SetScale(uiIcon->GetSize());
		uiIcon->AddDrawCall(iconDrawCall);
		uiIcon->SetAnchor(OSK::UI::Anchor::FULLY_CENTERED);
		uiIcon->SetMargin(Vector2f(5.0f, 0.0f));

		AddChild("icon", uiIcon);
	}

	// Renderizado de la versión.
	{
		auto* versionView = new OSK::UI::TextView({ 20.0f, 15.0f });
		versionView->SetFont(editorFont);
		versionView->SetFontSize(14);
		versionView->SetText(static_cast<std::string>(Engine::GetBuild()));
		versionView->AdjustSizeToText();
		versionView->SetMargin(Vector2f(10.0f, 0.0f));

		AddChild("version", versionView);
	}	

	// Renderizado del número de hilos.
	{
		auto* numThreadsView = new OSK::UI::TextView({ 20.0f, 15.0f });
		numThreadsView->SetFont(editorFont);
		numThreadsView->SetFontSize(14);
		numThreadsView->SetText(std::format("{} hilos", Engine::GetJobSystem()->GetNumThreads()));
		numThreadsView->AdjustSizeToText();
		numThreadsView->SetMargin(Vector2f(10.0f, 0.0f));

		AddChild("threads", numThreadsView);
	}

	// Renderizado de la GPU.
	{
		auto* gpuView = new OSK::UI::TextView({ 20.0f, 15.0f });
		gpuView->SetFont(editorFont);
		gpuView->SetFontSize(14);
		gpuView->SetText(std::format("{}", Engine::GetRenderer()->GetGpu()->GetName()));
		gpuView->AdjustSizeToText();
		gpuView->SetMargin(Vector2f(10.0f, 0.0f));

		AddChild("gpu", gpuView);
	}

	// Renderizado de FPS.
	{
		m_fpsView = new OSK::UI::TextView({ 50.0f, 15.0f });
		m_fpsView->SetFont(editorFont);
		m_fpsView->SetFontSize(14);
		m_fpsView->SetMargin(Vector2f(10.0f, 0.0f));
		m_fpsView->SetText("XXX fps");
		m_fpsView->AdjustSizeToText();

		AddChild("fps", m_fpsView);
	}
}

void OSK::Editor::UI::EditorBottomBar::OnSizeChanged(const Vector2f& previousSize) {
	auto& barDrawCall = GetAllDrawCalls().At(m_barDrawCallIndex);
	barDrawCall.transform.SetScale(GetSize());
}

void OSK::Editor::UI::EditorBottomBar::Update(TDeltaTime deltaTime) {
	m_accumulatedTime += deltaTime;
	m_frameCount++;

	if (m_accumulatedTime > 1.0f) {
		m_fpsView->SetText(std::format("{:.2f} fps", static_cast<TDeltaTime>(m_frameCount) / m_accumulatedTime));
		m_accumulatedTime = 0.0f;
		m_frameCount = 0;
	}
}
