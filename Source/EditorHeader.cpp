#include "EditorHeader.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"
#include "Texture.h"

#include "UiImageView.h"
#include "UiTextView.h"

#include "EditorUiConstants.h"


OSK::Editor::UI::EditorHeader::EditorHeader(const Vector2f& size) : OSK::UI::HorizontalContainer({ size.x, 40.0f }) {
	auto editorFont = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");
	const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
		->GetTextureView2D();

	SetMargin(Vector2f::Zero);

	auto iconTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/editor_icon.json");
	const auto iconView = &iconTexture->GetTextureView2D();

	GetSprite().SetImageView(uiView);
	GetSprite().color = Constants::DarkColor;

	OSK::UI::ImageView* uiIcon = new OSK::UI::ImageView(iconTexture->GetSize().ToVector2f() * (32.0f / iconTexture->GetSize().ToVector2f().y));
	uiIcon->GetSprite().SetImageView(iconView);
	uiIcon->SetAnchor(OSK::UI::Anchor::FULLY_CENTERED);
	uiIcon->SetMargin(Vector2f(5.0f, 0.0f));

	AddChild("icon", uiIcon);
}
