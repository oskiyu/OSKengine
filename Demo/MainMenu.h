#pragma once

#include <OSKengine/UiFreeContainer.h>
#include <OSKengine/UiButton.h>

class MainMenu : public OSK::UI::FreeContainer {

public:

	explicit MainMenu(const OSK::Vector2f& size);

	void ToMainMenu();

	constexpr static auto Name = "MainMenu";

private:

	void ToPanel(OSK::UI::IElement* panel);
	void ToCarSelection();

	OSK::OwnedPtr<OSK::UI::Button> CreateButton(const std::string& text);
	OSK::OwnedPtr<OSK::UI::IContainer> CreateButtonsContainer(float contentSize);
	OSK::OwnedPtr<OSK::UI::IElement> CreateStrip();
	OSK::OwnedPtr<OSK::UI::IElement> CreateTitle();

};
