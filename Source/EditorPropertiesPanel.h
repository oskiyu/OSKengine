#pragma once

#include "UiVerticalContainer.h"
#include "UiHorizontalContainer.h"
#include "UiTextView.h"

#include "ISystem.h"

namespace OSK::ECS {
	class ISystem;
}

namespace OSK::Editor::UI {

	class EditorPanelTitle;

	/// @brief Panel de propiedades.
	/// Su título será "Propiedades".
	class OSKAPI_CALL PropertiesPanel : public OSK::UI::VerticalContainer {

	public:

		constexpr static auto Name = "EditorPropertiesPanel";

		explicit PropertiesPanel(const Vector2f& size);

		void SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font);
		void SetFontSize(USize64 fontSize);

		/// @brief Establece el subtítulo del panel.
		/// @param text Texto del subtítulo.
		void SetSubtitle(const std::string& text);


		/// @brief Elimina todo el contendiro del panel.
		void ClearContent();

		/// @brief Muestra el contenido del panel.
		void ShowContent();


		/// @brief Establece el contenido del panel
		/// a partir de las propiedades del sistema.
		/// @param system Sistema a mostrar.
		void UpdateBySystem(const OSK::ECS::ISystem* system);

		/// @brief Establece el contenido del panel
		/// a partir de las propiedades del objeto.
		/// @param obj Objeto a mostrar.
		void UpdateByObject(const OSK::ECS::GameObjectIndex obj);

	private:

		EditorPanelTitle* m_title = nullptr;
		OSK::UI::TextView* m_subtitle = nullptr;

		OSK::ASSETS::AssetRef<OSK::ASSETS::Font> m_font;

		DynamicArray<OSK::UI::HorizontalContainer*> m_content;
		DynamicArray<OSK::UI::TextView*> m_textContent;

	};

}
