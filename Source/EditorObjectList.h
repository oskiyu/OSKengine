#pragma once

#include "UiVerticalContainer.h"
#include "UiButton.h"

#include "GameObject.h"

namespace OSK::Editor {
	class Editor;
}

namespace OSK::Editor::UI {

	class PropertiesPanel;
	class EditorPanelTitle;
	class ObjectPropertiesPanel;


	/// @brief Elemento de interfaz de usuario del editor
	/// que contiene informaci�n sobre los objetos de la escena.
	class OSKAPI_CALL ObjectList : public OSK::UI::VerticalContainer {

	public:

		constexpr static auto Name = "EditorObjectList";

		explicit ObjectList(
			const Vector2f& size,
			OSK::Editor::Editor* editor);

		/// @brief Limpia la informaci�n de todos
		/// los objetos.
		void ClearObjects();

		/// @brief Establece la lista de objetos
		/// disponibles.
		/// @param objects �ndices de los objetos.
		/// 
		/// @pre Los objetos de @p objects deben
		/// ser v�lidos.
		void SetObjects(std::span<const OSK::ECS::GameObjectIndex> objects);

		void SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font);
		void SetFontSize(USize64 fontSize);

		/// @brief Limpia la selecci�n actual,
		/// haciendo que ning�n objeto siga
		/// seleccionado.
		void ClearSelection();

		/// @return Panel de propiedades de un objeto
		/// en concreto.
		ObjectPropertiesPanel* GetPropertiesPanel();

		/// @return Objeto actualmente seleccionado.
		/// Puede devolver un objeto vac�o.
		ECS::GameObjectIndex GetSelectedObject() const;

	private:

		/// @brief Crea y a�ade el bot�n
		/// que permite a�adir nuevos objetos.
		/// @post Habr� un bot�n a�adido
		/// bajo el nombre `newObjectButton`.
		void AddCreateObjectButton();

		void OnSizeChanged(const Vector2f&) override;
			
		static constexpr USize32 MaxShownObjects = 10;

		UIndex64 m_backgroundDrawCallIndex = 0;

		ECS::GameObjectIndex m_selectedObject = ECS::GameObjectIndex::CreateEmpty();

		DynamicArray<OSK::UI::Button*> m_textViews{};
		EditorPanelTitle* m_title = nullptr;
		ObjectPropertiesPanel* m_propertiesPanel = nullptr;

		OSK::Editor::Editor* m_editorRef;

	};

}
