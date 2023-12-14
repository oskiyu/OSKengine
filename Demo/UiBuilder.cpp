#include "UiBuilder.h"

#include <OSKengine/AssetManager.h>
#include <OSKengine/OSKengine.h>

OSK::OwnedPtr<OSK::UI::Button> UiBuilder::CreateCarSelectionButton(std::string_view buttonDescFile) {
	auto button = new OSK::UI::Button(OSK::Vector2f(200.0f, 150.0f));

	/*button->SetType(OSK::UI::Button::Type::NORMAL);
	button->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);

	button->GetDefaultSprite().SetImageView(uiView);
	button->GetDefaultSprite().color = OSK::Color::White;
	
	button->GetSelectedSprite().SetImageView(uiView);
	button->GetSelectedSprite().color = OSK::Color::White;

	button->GetPressedSprite().SetImageView(uiView);
	button->GetPressedSprite().color = OSK::Color::White;*/

	return button;
}

OSK::OwnedPtr<OSK::UI::Button> UiBuilder::CreateMainMenuButton(std::string_view text) {
	OSK::ASSETS::AssetManager* assetsManager = OSK::Engine::GetAssetManager();

	auto buttonTexture = assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");

	auto button = new OSK::UI::Button(OSK::Vector2f(180.0f, 50.0f), static_cast<std::string>(text));

	button->SetKeepRelativeSize(true);
	button->SetTextFont(assetsManager->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json"));
	button->SetTextFontSize(32);
	button->SetType(OSK::UI::Button::Type::NORMAL);
	button->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);

	button->GetDefaultSprite().LinkAsset(buttonTexture);
	button->GetDefaultSprite().SetImageView(&buttonTexture->GetTextureView2D());
	button->GetDefaultSprite().color = OSK::Color(0.3f, 0.36f, 1.0f, 1.0f);

	button->GetSelectedSprite().LinkAsset(buttonTexture);
	button->GetSelectedSprite().SetImageView(&buttonTexture->GetTextureView2D());
	button->GetSelectedSprite().color = OSK::Color(0.3f, 0.5f, 1.0f, 1.0f);

	button->GetPressedSprite().LinkAsset(buttonTexture);
	button->GetPressedSprite().SetImageView(&buttonTexture->GetTextureView2D());
	button->GetPressedSprite().color = OSK::Color(0.5f, 0.7f, 1.0f, 1.0f);

	return button;
}
