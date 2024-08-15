#pragma once

#include "EditorPropertiesPanel.h"


namespace OSK::Editor::UI {

	/// @brief Panel que contiene las propiedades
	/// de un sistema ECS.
	class SystemPropertiesPanel : public PropertiesPanel {

	public:

		constexpr static auto Name = "SystemPropertiesPanel";

		explicit SystemPropertiesPanel(const Vector2f& size);

		/// @brief Establece el contenido del panel
		/// a partir de las propiedades del sistema.
		/// @param system Sistema a mostrar.
		void UpdateBySystem(const OSK::ECS::ISystem* system);

	private:

		DynamicArray<OSK::UI::HorizontalContainer*> m_content;

	};

}
