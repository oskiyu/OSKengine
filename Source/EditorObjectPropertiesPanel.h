#pragma once

#include "EditorPropertiesPanel.h"


namespace OSK::Editor::UI {

	/// @brief Panel que contiene las propiedades
	/// (componentes) de un objeto ECS.
	class ObjectPropertiesPanel : public PropertiesPanel {

	public:

		constexpr static auto Name = "ObjectPropertiesPanel";

		explicit ObjectPropertiesPanel(const Vector2f& size);

		/// @brief Establece el contenido del panel
		/// a partir de las propiedades del objeto.
		/// @param obj Objeto a mostrar.
		void SetObject(const OSK::ECS::GameObjectIndex obj);

		/// @brief Añade la vista de un componente.
		/// @param view Vista de un componente.
		void AddComponentView(OwnedPtr<OSK::Editor::Views::IComponentView> view);

		/// @brief Quita todas las vistas de componentes.
		void ClearAllComponentViews();

	private:

		void EmplaceChild(IElement* child) override;

		/// @brief Índice de la siguiente vista.
		/// Para darle a cada vista un nombre único.
		UIndex32 m_nextViewIndex = 0;

	};

}
