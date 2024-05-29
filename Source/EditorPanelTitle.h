#pragma once

#include "UiVerticalContainer.h"
#include "Font.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::UI {

	/// @brief Título de un panel, con un subrayado azul.
	class OSKAPI_CALL EditorPanelTitle : public OSK::UI::VerticalContainer {

	public:

		explicit EditorPanelTitle(const OSK::Vector2f& size);

		void SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font);
		void SetFontSize(USize64 fontSize);

		void SetText(const std::string& text);

	private:

		OSK::UI::TextView* m_title = nullptr;

	};

}
