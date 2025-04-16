#include "EditorPanelTitle.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "UiTextView.h"
#include "UiImageView.h"
#include "EditorUiConstants.h"


OSK::Editor::UI::EditorPanelTitle::EditorPanelTitle(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
	const Vector2f textSize = { size.x, 25.0f };

	auto editorFont = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_title = new OSK::UI::TextView(textSize);
	m_title->SetMargin(Vector4f(0.0f));
	m_title->SetPadding(Vector2f(5.0f));
	m_title->SetFont(editorFont);
	m_title->SetFontSize(Constants::TitleFontSize);
	m_title->SetText("Objetos");

	// Fondo.
	{
		GRAPHICS::SdfDrawCall2D barDrawCall{};
		barDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		barDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		barDrawCall.mainColor = Constants::DarkColor;
		barDrawCall.transform.SetScale(GetSize());

		m_backgroundDrawCallIndex = m_title->GetAllDrawCalls().GetSize();

		m_title->AddDrawCall(barDrawCall);
	}

	AddChild("title", UniquePtr<OSK::UI::IElement>(m_title));

	// Línea.
	{
		GRAPHICS::SdfDrawCall2D titleLineDrawCall{};
		titleLineDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		titleLineDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		titleLineDrawCall.mainColor = Constants::DetailsColor;
		titleLineDrawCall.transform.SetScale({ size.x, 5.0f });

		m_line = new OSK::UI::ImageView({ size.x, 5.0f });
		m_line->SetMargin(Vector4f(0.0f));

		m_lineDrawCallIndex = m_line->GetAllDrawCalls().GetSize();

		m_line->AddDrawCall(titleLineDrawCall);

		AddChild("titleLine", UniquePtr<OSK::UI::IElement>(m_line));
	}

	SetMargin(Vector4f(5.0f));
	SetPadding(Vector2f(0.0f));

	AdjustSizeToChildren();
}

void OSK::Editor::UI::EditorPanelTitle::OnSizeChanged(const Vector2f&) {
	auto& backgroundDrawCall = m_title->GetAllDrawCalls()[m_backgroundDrawCallIndex];
	backgroundDrawCall.transform.SetScale(GetSize());
	
	auto& lineDrawCall = m_line->GetAllDrawCalls()[m_lineDrawCallIndex];
	lineDrawCall.transform.SetScale({ m_line->GetSize().x, 5.0f });
}

void OSK::Editor::UI::EditorPanelTitle::SetText(const std::string& text) {
	m_title->SetText(text);
}
