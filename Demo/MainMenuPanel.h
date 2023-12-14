#pragma once

#include <OSKengine/UiFreeContainer.h>
#include <OSKengine/UiButton.h>

class MainMenu;

class MainMenuPanel : public OSK::UI::FreeContainer {

public:

	explicit MainMenuPanel(const OSK::Vector2f& size, MainMenu* parent);

	void SetTitle(const std::string& text);
	void SetText(const std::string& text);

	constexpr static auto Name = "MainMenuPanel";

private:

	OSK::OwnedPtr<OSK::UI::Button> CreateBackButton();
	OSK::OwnedPtr<OSK::UI::IElement> CreateTitle(const std::string& text);
	OSK::OwnedPtr<OSK::UI::IElement> CreateText(const std::string& text);

	MainMenu* m_parent = nullptr;

};
