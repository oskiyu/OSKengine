#include "EditorPropertiesPanel.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorPanelTitle.h"
#include "EditorUiConstants.h"


OSK::Editor::UI::PropertiesPanel::PropertiesPanel(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
	const Vector2f textSize = { size.x - 20.0f, 25.0f };

	m_title = new EditorPanelTitle(textSize);
	m_title->SetMargin(Vector2f(5.0f));
	m_title->SetPadding(Vector2f(5.0f));
	m_title->SetText("Propiedades");

	AddChild("title", m_title);

	m_subtitle = new OSK::UI::TextView(textSize);
	m_subtitle->SetMargin(Vector4f(5.0f));
	m_subtitle->SetPadding(Vector2f(5.0f));

	AddChild("subtitle", m_subtitle);

	GRAPHICS::SdfDrawCall2D backgroundDrawCall{};
	backgroundDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
	backgroundDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
	backgroundDrawCall.mainColor = Constants::BackgroundAlternativeColor;
	backgroundDrawCall.transform.SetScale(GetSize());

	// AddDrawCall(backgroundDrawCall);
}


void OSK::Editor::UI::PropertiesPanel::SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font) {
	m_font = font;

	m_title->SetFont(font);
	m_subtitle->SetFont(font);
}

void OSK::Editor::UI::PropertiesPanel::SetFontSize(USize64 fontSize) {
	m_title->SetFontSize(fontSize + 2);
	m_subtitle->SetFontSize(fontSize - 2);
}

void OSK::Editor::UI::PropertiesPanel::SetSubtitle(const std::string& text) {
	m_subtitle->SetText(text);
}

void OSK::Editor::UI::PropertiesPanel::ClearContent() {
	m_subtitle->SetInvisible();
}

void OSK::Editor::UI::PropertiesPanel::ShowContent() {
	m_subtitle->SetVisible();
}
