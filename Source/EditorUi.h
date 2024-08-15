#pragma once

/**/
#include "ApiCall.h"
#include "NumericTypes.h"

#include "Vector2.hpp"

#include "UiBorderLayout.h"

namespace OSK::ECS {
	class EntityComponentSystem;
}

namespace OSK::Editor {
	class Editor;
}

namespace OSK::Editor::UI {

	class ObjectList;
	class SystemList;
	class EditorBottomBar;


	/// @brief Clase principal de la interfaz del editor.
	class OSKAPI_CALL EditorUi : public OSK::UI::BorderLayout {

	public:

		static constexpr auto Name = "EditorUi";

		explicit EditorUi(
			const Vector2f& size,
			OSK::Editor::Editor* editor);

		/// @brief Actualiza la interfaz de todo el editor.
		/// @param ecs Clase ECS del motor.
		/// @param deltaTime Tiempo desde la última actualización.
		void Update(
			const OSK::ECS::EntityComponentSystem* ecs,
			TDeltaTime deltaTime);

		ObjectList* GetObjectListPanel();

	private:

		ObjectList* m_objectListPanel = nullptr;
		SystemList* m_systemListPanel = nullptr;
		EditorBottomBar* m_bottomBar = nullptr;

	};

}
/**/
