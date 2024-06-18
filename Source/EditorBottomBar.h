#pragma once
/**/
#include "UiHorizontalContainer.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::UI {

	/// @brief Barra inferior del editor.
	/// Contiene:
	/// - Icono.
	/// - Versión.
	/// - Número de hilos.
	/// - GPU.
	/// - FPS (actualizado cada segundo).
	class OSKAPI_CALL EditorBottomBar : public OSK::UI::HorizontalContainer {

	public:

		explicit EditorBottomBar(const Vector2f& size);

		void Update(TDeltaTime deltaTime);

	protected:

		void OnSizeChanged(const Vector2f& previousSize) override;

	private:

		/// @note No será null.
		OSK::UI::TextView* m_fpsView = nullptr;
		TDeltaTime m_accumulatedTime = 0.0f;
		UIndex32 m_frameCount = 0;

		UIndex64 m_barDrawCallIndex = 0;

	};

}
/**/