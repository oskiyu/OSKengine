/*
module;

#include "UiBorderLayout.h"
#include "Vector2.hpp"

namespace OSK::ECS {
	class EntityComponentSystem;
}

namespace OSK::Editor::UI {
	class ObjectList;
	class SystemList;
}

export module OSKengine.Editor.Ui;

namespace OSK::Editor::UI {
	class EditorBottomBar;
}

namespace OSK::Editor::UI {

	export class OSKAPI_CALL EditorUi : public OSK::UI::BorderLayout {

	public:

		static constexpr auto Name = "EditorUi";

		explicit EditorUi(const Vector2f& size);

		void Update(const ::OSK::ECS::EntityComponentSystem* ecs, TDeltaTime deltaTime);

	private:

		ObjectList* m_objectListPanel = nullptr;
		SystemList* m_systemListPanel = nullptr;
		EditorBottomBar* m_bottomBar = nullptr;

	};

}
/**/