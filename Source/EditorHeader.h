#pragma once

#include "UiHorizontalContainer.h"

namespace OSK::Editor::UI {

	/// @brief Barra superior del editor.
	/// Contiene:
	/// - Logo.
	class OSKAPI_CALL EditorHeader : public OSK::UI::HorizontalContainer {

	public:

		explicit EditorHeader(const Vector2f& size);

	protected:

		void OnSizeChanged(const Vector2f& previousSize) override;

	private:

		UIndex64 m_barDrawCallIndex = 0;

	};

}
