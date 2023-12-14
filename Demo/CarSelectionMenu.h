#pragma once

#include <OSKengine/UiFreeContainer.h>
#include <OSKengine/UiButton.h>

#include <OSKengine/Texture.h>


class CarSelectionMenu : public OSK::UI::FreeContainer {

public:

	explicit CarSelectionMenu(const OSK::Vector2f& size);

	constexpr static auto Name = "CarSelectionMenu";

private:

	OSK::OwnedPtr<OSK::UI::IContainer> CreateTopContainer(const OSK::Vector2f& size) const;
	OSK::OwnedPtr<OSK::UI::IContainer> CreateBottomContainer(const OSK::Vector2f& size) const;

	OSK::OwnedPtr<OSK::UI::Button> CreateButton(OSK::ASSETS::AssetRef<OSK::ASSETS::Texture> texture) const;

	std::string m_currentlySelectedCar;

};
