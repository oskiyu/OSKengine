#pragma once

#include "EditorPropertiesPanel.h"
#include "OwnedPtr.h"
#include "ISystemView.h"

namespace OSK::ECS {
	class ISystem;
}

namespace OSK::Editor::UI {

	/// @brief Panel que contiene las propiedades
	/// de un sistema ECS.
	class SystemPropertiesPanel : public PropertiesPanel {

	public:

		constexpr static auto Name = "SystemPropertiesPanel";

		explicit SystemPropertiesPanel(const Vector2f& size);

		/// @brief Establece la vista con las
		/// propiedades del sistema.
		/// Si ya tenía una vista, la reemplaza.
		/// @param view Vista.
		void SetView(OwnedPtr<OSK::Editor::Views::ISystemView> view);

		/// @brief Elimina la vista de propiedades
		/// del sistema. Si no hay, no hace nada.
		void ClearView();
		void ClearContent() override;

		/// @brief Establece el sistema seleccionado, incluso
		/// si no tiene enlazada una vista.
		/// @param system Sistema seleccionado.
		/// Nullptr si no hay ninguno seleccionado.
		void SetSelectedSystem(ECS::ISystem* system);

	private:

		/// @brief Crea y añade el botón
		/// que permite cambiar el estado
		/// (activo o inactivo) del sistema.
		/// @post Habrá un botón añadido
		/// bajo el nombre `stateButton`.
		void AddStateObjectButton();

		OSK::UI::Button* m_stateButton = nullptr;
		ECS::ISystem* m_currentlySelectedSystem = nullptr;

		constexpr static auto ChildName = "content";

	};

}
