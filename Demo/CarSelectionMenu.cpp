#include "CarSelectionMenu.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/UiFreeContainer.h>
#include <OSKengine/UiHorizontalContainer.h>

#include "CarSelectPanel.h"
#include "MenuEvents.h"
#include "CarRegistry.h"


CarSelectionMenu::CarSelectionMenu(const OSK::Vector2f& size) : OSK::UI::FreeContainer(size) {
	auto* assetManager = OSK::Engine::GetAssetManager();

	// --- INIT --- //

	auto chasGtTexture = assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/ChasGT.json");
	auto owTexture = assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/OW78.json");
	auto lwmTexture = assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/LWMT5.json");


	SetKeepRelativeSize(true);

	const auto selectionImageRatio = size / chasGtTexture->GetSize().ToVector2f();
	const OSK::Vector2f selectionImageSize = size * (OSK::Vector2f::One / selectionImageRatio);

	auto bottomContainer = CreateBottomContainer(size);

	AddChild("bottom", bottomContainer.GetPointer());

	// --- BUTTONS --- //

	auto chasButtonTexture	= assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/ChasButton.json");
	auto owButtonTexture	= assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/OwButton.json");
	auto lwmButtonTexture	= assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/LwmButton.json");
	auto buttonTexture		= assetManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/PlayButton.json");

	auto chasButton = CreateButton(chasButtonTexture);
	auto owButton	= CreateButton(owButtonTexture);
	auto lwmButton = CreateButton(lwmButtonTexture);

	chasButton->SetCallback([this](bool) {
		m_currentlySelectedCar = (std::string)CarRegistry::GetAssetsRoute("ChasGT");
		OSK::Engine::GetEcs()->PublishEvent(SelectCarEvent{ m_currentlySelectedCar });
		});

	owButton->SetCallback([this](bool) {
		m_currentlySelectedCar = (std::string)CarRegistry::GetAssetsRoute("OW-78");
		OSK::Engine::GetEcs()->PublishEvent(SelectCarEvent{ m_currentlySelectedCar });
		});

	lwmButton->SetCallback([this](bool) {
		m_currentlySelectedCar = (std::string)CarRegistry::GetAssetsRoute("LWM 0");
		OSK::Engine::GetEcs()->PublishEvent(SelectCarEvent{ m_currentlySelectedCar });
		});

	auto playButton = CreateButton(buttonTexture);
	playButton->SetCallback([this](bool) {
		this->SetInvisible();
		OSK::Engine::GetEcs()->PublishEvent(LoadLevelEvent{ m_currentlySelectedCar });
		});


	bottomContainer->AddChild("chas_button", chasButton.GetPointer());
	bottomContainer->AddChild("ow_button", owButton.GetPointer());
	bottomContainer->AddChild("lwm_button", lwmButton.GetPointer());

	bottomContainer->AddChild("play_button", playButton.GetPointer());

	m_currentlySelectedCar = (std::string)CarRegistry::GetAssetsRoute("LWM 0");
}

OSK::OwnedPtr<OSK::UI::IContainer> CarSelectionMenu::CreateTopContainer(const OSK::Vector2f& size) const {
	auto topContainer = new OSK::UI::FreeContainer(size);
	topContainer->SetMargin(OSK::Vector2f::Zero);
	topContainer->SetPadding(OSK::Vector2f::Zero);
	topContainer->SetKeepRelativeSize(true);
	topContainer->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);

	return topContainer;
}

OSK::OwnedPtr<OSK::UI::IContainer> CarSelectionMenu::CreateBottomContainer(const OSK::Vector2f& size) const {
	auto fondoTexture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/Selection/Fondo.json");
	const float selectionImageRatio = (size.x / fondoTexture->GetSize().ToVector2f().x);

	auto selectionButtonContainer = new OSK::UI::HorizontalContainer(fondoTexture->GetSize().ToVector2f() * selectionImageRatio);
	selectionButtonContainer->SetMargin(OSK::Vector2f::Zero);
	selectionButtonContainer->SetPadding(OSK::Vector2f::Zero);
	// selectionButtonContainer->GetSprite().LinkAsset(fondoTexture);
	// selectionButtonContainer->GetSprite().SetImageView(&fondoTexture->GetTextureView2D());
	selectionButtonContainer->SetKeepRelativeSize(true);
	selectionButtonContainer->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::BOTTOM);

	return selectionButtonContainer;
}

OSK::OwnedPtr<OSK::UI::Button> CarSelectionMenu::CreateButton(OSK::ASSETS::AssetRef<OSK::ASSETS::Texture> texture) const {
	auto button = new OSK::UI::Button(OSK::Vector2f(230.0f, 150.0f) * 0.5f, "");

	button->SetKeepRelativeSize(true);
	button->SetType(OSK::UI::Button::Type::NORMAL);
	button->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);

	button->SetMargin(OSK::Vector2f(50.0f, 10.0f));

	button->GetDefaultSprite().LinkAsset(texture);
	button->GetDefaultSprite().SetImageView(&texture->GetTextureView2D());
	button->GetDefaultSprite().color = OSK::Color::White;

	button->GetSelectedSprite().LinkAsset(texture);
	button->GetSelectedSprite().SetImageView(&texture->GetTextureView2D());
	button->GetSelectedSprite().color = OSK::Color(0.3f, 0.5f, 1.0f, 1.0f);

	button->GetPressedSprite().LinkAsset(texture);
	button->GetPressedSprite().SetImageView(&texture->GetTextureView2D());
	button->GetPressedSprite().color = OSK::Color(0.5f, 0.7f, 1.0f, 1.0f);

	button->SetVisible();

	return button;
}
