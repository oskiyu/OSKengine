#pragma once

/**/
#include "ApiCall.h"
#include "NumericTypes.h"

#include "Vector2.hpp"

#include "UiBorderLayout.h"

namespace OSK::ECS {
	class EntityComponentSystem;
	class ISystem;
}

namespace OSK::Editor {
	class Editor;

	namespace Views {
		class IComponentView;
		class ISystemView;
	}
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

		/// @brief Actualiza la interfaz del editor.
		/// @param ecs Clase ECS del motor.
		/// @param deltaTime Tiempo desde la última actualización.
		void Update(
			const OSK::ECS::EntityComponentSystem* ecs,
			TDeltaTime deltaTime);


		/// @brief Elimina todas las vistas de los
		/// componentes de un objeto ECS, localizadas
		/// en el panel de propiedades del objeto.
		void ClearAllComponentViews();

		/// @brief Añade la vista de un componente al 
		/// panel de propiedades del objeto.
		/// @param view Nueva vista.
		void AddComponentView(OwnedPtr<OSK::Editor::Views::IComponentView> view);

		/// @brief Establece la vista de las propiedades
		/// del sistema seleccionado.
		/// @param view Vista con las propiedades.
		void SetSystemPropertiesView(OwnedPtr<OSK::Editor::Views::ISystemView> view);

		/// @brief Elimina la vista de las propiedades
		/// del sistema seleccionado.
		/// Si ya se había eliminado no ocurre nada.
		void ClearSystemPropertiesView();

		/// @brief Establece el sistema seleccionado, incluso
		/// si no tiene enlazada una vista.
		/// @param system Sistema seleccionado.
		/// Nullptr si no hay ninguno seleccionado.
		void SetSelectedSystem(ECS::ISystem* system);

	private:

		ObjectList* m_objectListPanel = nullptr;
		SystemList* m_systemListPanel = nullptr;
		EditorBottomBar* m_bottomBar = nullptr;

	};

}

