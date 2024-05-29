#pragma once

#include "UiVerticalContainer.h"
#include "UiButton.h"

#include "GameObject.h"

namespace OSK::Editor::UI {

	class PropertiesPanel;
	class EditorPanelTitle;


	class OSKAPI_CALL ObjectList : public OSK::UI::VerticalContainer {

	public:

		constexpr static auto Name = "EditorObjectList";

		explicit ObjectList(const Vector2f& size);

		void ClearObjects();
		void SetObjects(std::span<const OSK::ECS::GameObjectIndex> objects);

		void SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font);
		void SetFontSize(USize64 fontSize);

		void ClearSelection();

	private:

		static constexpr USize32 MaxShownObjects = 15;

		DynamicArray<OSK::UI::Button*> m_textViews{};
		EditorPanelTitle* m_title = nullptr;
		PropertiesPanel* m_propertiesPanel = nullptr;

	};

}
