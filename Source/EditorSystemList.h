#pragma once

#include "UiVerticalContainer.h"
#include "UiTextView.h"
#include "UiButton.h"

#include "ISystem.h"

namespace OSK::Editor {
	class Editor;

	namespace Views {
		class ISystemView;
	}
}

namespace OSK::Editor::UI {

	class EditorPanelTitle;
	class SystemPropertiesPanel;

	class OSKAPI_CALL SystemList : public OSK::UI::VerticalContainer {

	public:

		constexpr static auto Name = "EditorSystemList";

		explicit SystemList(
			const Vector2f& size,
			OSK::Editor::Editor* editorRef);


		/// @brief Quita los botones de los sistemas.
		void ClearSystems();

		/// @brief Establece los botones de los sistemas
		/// para los sistemas dados.
		/// @param systems Sisteas a establecer.
		void SetSystems(std::span<const OSK::ECS::ISystem*> systems);

		/// @brief Deselecciona el sistema seleccionado.
		/// Si no hay ningún sistema seleccionado,
		/// no hace nada.
		void ClearSelection();

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

		void OnSizeChanged(const Vector2f&) override;

		static constexpr USize32 MaxShownSystems = 10;

		UIndex64 m_backgroundDrawCallIndex = 0;

		DynamicArray<OSK::UI::Button*> m_textViews{};
		EditorPanelTitle* m_title = nullptr;
		SystemPropertiesPanel* m_propertiesPanel = nullptr;

		OSK::Editor::Editor* m_editorRef = nullptr;

	};

}
