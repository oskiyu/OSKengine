#pragma once

#include <OSKengine/OwnedPtr.h>
#include <OSKengine/UiButton.h>


class UiBuilder {

public:

	static OSK::OwnedPtr<OSK::UI::Button> CreateCarSelectionButton(std::string_view buttonDescFile);
	static OSK::OwnedPtr<OSK::UI::Button> CreateMainMenuButton(std::string_view text);

};
