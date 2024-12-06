#pragma once

#include "Color.hpp"

namespace OSK::Editor::UI::Constants {

	constexpr static Color BackgroundColor = Color(0.3f, 0.3f, 0.3f);
	constexpr static Color BackgroundAlternativeColor = Color(0.25f, 0.25f, 0.25);

	constexpr static Color DarkColor = Color(0.2f, 0.2f, 0.2f);

	constexpr static Color HoveredColor = Color(0.3f, 0.3f, 0.45f);
	constexpr static Color SelectedColor = Color(0.3f, 0.3f, 0.65f);

	constexpr static Color DetailsColor = Color(51.0f / 255.0f, 102.0f / 255.0f, 1.0f);

	constexpr static Color DefaultGreenColor = Color::FromBytes(92, 153, 80);
	constexpr static Color HoveredGreenColor = Color::FromBytes(88, 168, 72);
	constexpr static Color SelectedGreenColor = Color::FromBytes(52, 145, 33);

	constexpr static Color DefaultRedColor = Color::FromBytes(150, 62, 62);
	constexpr static Color HoveredRedColor = Color::FromBytes(179, 46, 46);
	constexpr static Color SelectedRedColor = Color::FromBytes(148, 15, 15);

	constexpr static auto EditorFontPath = "Resources/Assets/Fonts/font1.json";


	constexpr static USize32 TitleFontSize = 18;
	constexpr static USize32 SubtitleFontSize = 15;
	constexpr static USize32 MainFontSize = 14;
	constexpr static USize32 SecondaryFontSize = 12;

}
