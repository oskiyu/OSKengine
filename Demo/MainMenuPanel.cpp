#include "MainMenuPanel.h"

#include "MainMenu.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/AssetManager.h>


MainMenuPanel::MainMenuPanel(const OSK::Vector2f& size, MainMenu* parent) : OSK::UI::FreeContainer(size), m_parent(parent) {
	auto texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");

	SetKeepRelativeSize(true);
	GetSprite().LinkAsset(texture);
	GetSprite().SetImageView(&texture->GetTextureView2D());
	GetSprite().color = OSK::Color(0.4f, 0.45f, 0.7f, 0.98f);

	AddChild("back", CreateBackButton().GetPointer());
}

void MainMenuPanel::SetTitle(const std::string& text) {
	AddChild("title", CreateTitle(text).GetPointer());
}

OSK::OwnedPtr<OSK::UI::Button> MainMenuPanel::CreateBackButton() {
	auto aboutBackButton = new OSK::UI::Button({ 100.0f, 50.0f });

	auto texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");

	aboutBackButton->SetKeepRelativeSize(true);
	aboutBackButton->SetTextFont(OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json"));
	aboutBackButton->SetTextFontSize(32);
	aboutBackButton->SetType(OSK::UI::Button::Type::NORMAL);
	aboutBackButton->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);

	aboutBackButton->GetDefaultSprite().LinkAsset(texture);
	aboutBackButton->GetDefaultSprite().SetImageView(&texture->GetTextureView2D());
	aboutBackButton->GetDefaultSprite().color = OSK::Color(0.3f, 0.36f, 1.0f, 1.0f);

	aboutBackButton->GetSelectedSprite().LinkAsset(texture);
	aboutBackButton->GetSelectedSprite().SetImageView(&texture->GetTextureView2D());
	aboutBackButton->GetSelectedSprite().color = OSK::Color(0.3f, 0.5f, 1.0f, 1.0f);

	aboutBackButton->GetPressedSprite().LinkAsset(texture);
	aboutBackButton->GetPressedSprite().SetImageView(&texture->GetTextureView2D());
	aboutBackButton->GetPressedSprite().color = OSK::Color(0.5f, 0.7f, 1.0f, 1.0f);

	aboutBackButton->SetText("Back");
	aboutBackButton->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::BOTTOM);
	aboutBackButton->SetCallback([&](bool) { m_parent->ToMainMenu(); });

	return aboutBackButton;
}

OSK::OwnedPtr<OSK::UI::IElement> MainMenuPanel::CreateTitle(const std::string& text) {
	auto aboutTitle = new OSK::UI::TextView(OSK::Vector2f(100.0f, 50.0f));

	aboutTitle->SetFont(OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json"));
	aboutTitle->SetFontSize(32);
	aboutTitle->SetText(text);
	aboutTitle->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);

	return aboutTitle;
}

void MainMenuPanel::SetText(const std::string& text) {
	AddChild("content", CreateText(text).GetPointer());
}

OSK::OwnedPtr<OSK::UI::IElement> MainMenuPanel::CreateText(const std::string& text) {
	auto content = new OSK::UI::TextView({ 400.0f, 50.0f });

	content->SetFont(OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json"));
	content->SetFontSize(20);
	content->SetText(std::format("OSKengine by oskiyu\n\tBuild {}\n\t(c) 2019 - 2023\n\n{}", OSK::Engine::GetBuild(),
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"));
	
	content->AdjustSizeToText();

	return content;
}
