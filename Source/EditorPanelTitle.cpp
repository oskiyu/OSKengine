#include "EditorPanelTitle.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "UiTextView.h"
#include "EditorUiConstants.h"


OSK::Editor::UI::EditorPanelTitle::EditorPanelTitle(const Vector2f& size) : OSK::UI::VerticalContainer(size) {
	auto uiTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");
	const auto uiView = &uiTexture->GetTextureView2D();

	const Vector2f textSize = { size.x, 25.0f };

	m_title = new OSK::UI::TextView(textSize);
	m_title->SetMargin(Vector4f(0.0f));
	m_title->SetPadding(Vector2f(5.0f));
	m_title->SetText("Objetos");

	m_title->GetSprite().SetImageView(uiView);
	m_title->GetSprite().color = Constants::DarkColor;

	AddChild("title", m_title);

	auto* titleLine = new OSK::UI::ImageView({ size.x, 5.0f });
	titleLine->SetMargin(Vector4f(0.0f));
	titleLine->GetSprite().SetImageView(uiView);
	titleLine->GetSprite().color = Constants::DetailsColor;

	AddChild("titleLine", titleLine);

	AdjustSizeToChildren();
}

void OSK::Editor::UI::EditorPanelTitle::SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font) {
	m_title->SetFont(font);
}

void OSK::Editor::UI::EditorPanelTitle::SetFontSize(USize64 fontSize) {
	m_title->SetFontSize(fontSize);
}

void OSK::Editor::UI::EditorPanelTitle::SetText(const std::string& text) {
	m_title->SetText(text);
}
