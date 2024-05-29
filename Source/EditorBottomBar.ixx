/*module;

#include "UiHorizontalContainer.h"

namespace OSK::UI {
	class TextView;
}

export module OSKengine.Editor.Ui:BottomBar;

namespace OSK::Editor::UI {

	export class OSKAPI_CALL EditorBottomBar : public OSK::UI::HorizontalContainer {

	public:

		explicit EditorBottomBar(const Vector2f& size);

		void Update(TDeltaTime deltaTime);

	private:

		OSK::UI::TextView* m_fpsView = nullptr;
		TDeltaTime m_accumulatedTime = 0.0f;
		UIndex32 m_frameCount = 0;

	};

}
/**/