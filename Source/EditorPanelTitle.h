#pragma once

#include "UiVerticalContainer.h"
#include "Font.h"

namespace OSK::UI {
	class TextView;
	class ImageView;
}

namespace OSK::Editor::UI {

	/// @brief Título de un panel, con un subrayado azul.
	class OSKAPI_CALL EditorPanelTitle : public OSK::UI::VerticalContainer {

	public:

		explicit EditorPanelTitle(const OSK::Vector2f& size);

		void SetText(const std::string& text);

	private:

		void OnSizeChanged(const Vector2f&) override;

		UIndex64 m_backgroundDrawCallIndex = 0;
		UIndex64 m_lineDrawCallIndex = 0;

		OSK::UI::TextView* m_title = nullptr;
		OSK::UI::ImageView* m_line = nullptr;

	};

}
