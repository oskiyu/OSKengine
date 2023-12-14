#include "MainMenu.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/UiHorizontalContainer.h>

#include "MenuEvents.h"
#include "MainMenuPanel.h"
#include "CarSelectionMenu.h"


MainMenu::MainMenu(const OSK::Vector2f& size) : OSK::UI::FreeContainer(size) {
	auto buttonContainer = CreateButtonsContainer(GetContentSize().x);

	SetKeepRelativeSize(true);

	AddChild("line", CreateStrip().GetPointer());
	AddChild("buttonContainer", buttonContainer.GetPointer());
	AddChild("title", CreateTitle().GetPointer());

	auto playButton = CreateButton("PLAY");
	playButton->SetCallback([this](bool) {
		ToCarSelection();
		});

	auto aboutButton = CreateButton("ABOUT");
	aboutButton->SetCallback([this](bool) {
		ToPanel(GetChild("about"));
		});

	auto optionsButton = CreateButton("AJUSTES");
	optionsButton->SetCallback([this](bool) {
		ToPanel(GetChild("options"));
		});

	auto exitButton = CreateButton("EXIT");
	exitButton->SetCallback([](bool) { OSK::Engine::GetEcs()->PublishEvent(ExitEvent()); });


	buttonContainer->AddChild("playButton", playButton.GetPointer());
	buttonContainer->AddChild("aboutButton", aboutButton.GetPointer());
	buttonContainer->AddChild("optionsButton", optionsButton.GetPointer());
	buttonContainer->AddChild("exitButton", exitButton.GetPointer());


	// Panels
	auto about = new MainMenuPanel(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.8f, this);
	about->SetTitle("About");
	about->SetText(std::format("OSKengine by oskiyu\n\tBuild{}\n\t(c) 2019 - 2023\n\n{}", OSK::Engine::GetBuild(),
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"));

	auto options = new MainMenuPanel(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.8f, this);
	options->SetTitle("Options");
	options->SetText(std::format("OSKengine by oskiyu\n\tBuild{}\n\t(c) 2019 - 2023\n\n{}", OSK::Engine::GetBuild(),
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"));

	about->SetInvisible();
	options->SetInvisible();

	AddChild("about", about);
	AddChild("options", options);


	// Selection menu

	auto selectionMenu = new CarSelectionMenu(size);
	selectionMenu->SetInvisible();
	AddChild(CarSelectionMenu::Name, selectionMenu);


	buttonContainer->AdjustSizeToChildren();
	buttonContainer->Rebuild();

	this->Rebuild();
}


OSK::OwnedPtr<OSK::UI::IContainer> MainMenu::CreateButtonsContainer(float contentSize) {
	OSK::OwnedPtr<OSK::UI::HorizontalContainer> mainMenuButtons = new OSK::UI::HorizontalContainer({ contentSize, 20.0f });
	mainMenuButtons->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::BOTTOM);
	mainMenuButtons->SetKeepRelativeSize(true);

	return mainMenuButtons.GetPointer();
}

OSK::OwnedPtr<OSK::UI::IElement> MainMenu::CreateStrip() {
	OSK::OwnedPtr<OSK::UI::FreeContainer> menuLine = new OSK::UI::FreeContainer({ 2000.0f, 85.0f });

	auto buttonTexture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");
	
	menuLine->SetKeepRelativeSize(true);
	menuLine->GetSprite().SetImageView(&buttonTexture->GetTextureView2D());
	menuLine->GetSprite().color = OSK::Color(0.3f, 0.3f, 0.9f, 0.8f);
	menuLine->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::BOTTOM);
	menuLine->SetMargin(OSK::Vector2f(0.0f, 550.0f));
	
	return menuLine.GetPointer();
}

OSK::OwnedPtr<OSK::UI::IElement> MainMenu::CreateTitle() {
	OSK::OwnedPtr<OSK::UI::FreeContainer> titleContainer = new OSK::UI::FreeContainer({ 1200.0f, 600.0f });
	titleContainer->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);

	auto titleTexture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/GameTitle.json");

	OSK::OwnedPtr<OSK::UI::ImageView> uiTitle = new OSK::UI::ImageView(titleTexture->GetSize().ToVector2f() * 0.4f);
	uiTitle->SetKeepRelativeSize(true);
	uiTitle->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);
	uiTitle->GetSprite().SetImageView(&titleTexture->GetTextureView2D());
	uiTitle->GetSprite().LinkAsset(titleTexture);
	uiTitle->SetMargin(OSK::Vector4f(25.0f));

	titleContainer->AddChild("title", uiTitle.GetPointer());
	titleContainer->SetKeepRelativeSize(true);

	return titleContainer.GetPointer();
}

void MainMenu::ToPanel(OSK::UI::IElement* panel) {
	GetChild("line")->SetInvisible();
	GetChild("buttonContainer")->SetInvisible();
	GetChild("title")->SetInvisible();

	panel->SetVisible();
}

void MainMenu::ToCarSelection() {
	GetChild("line")->SetInvisible();
	GetChild("buttonContainer")->SetInvisible();
	GetChild("title")->SetInvisible();

	GetChild(CarSelectionMenu::Name)->SetVisible();
}

OSK::OwnedPtr<OSK::UI::Button> MainMenu::CreateButton(const std::string& text) {
	OSK::ASSETS::AssetManager* assetsManager = OSK::Engine::GetAssetManager();

	auto buttonTexture = assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");

	auto button = new OSK::UI::Button(OSK::Vector2f(180.0f, 50.0f), text);

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

void MainMenu::ToMainMenu() {
	GetChild("line")->SetVisible();
	GetChild("buttonContainer")->SetVisible();
	GetChild("title")->SetVisible();

	GetChild("about")->SetInvisible();
	GetChild("options")->SetInvisible();
}
