#pragma once

#include "UiVerticalContainer.h"
#include "UiTextView.h"
#include "UiButton.h"

#include "ISystem.h"

namespace OSK::Editor::UI {

	class EditorPanelTitle;
	class PropertiesPanel;

	class OSKAPI_CALL SystemList : public OSK::UI::VerticalContainer {

	public:

		constexpr static auto Name = "EditorSystemList";

		explicit SystemList(const Vector2f& size);

		void ClearSystems();
		void SetSystems(std::span<const OSK::ECS::ISystem*> systems);

		void SetFont(OSK::ASSETS::AssetRef<OSK::ASSETS::Font> font);
		void SetFontSize(USize64 fontSize);

		void ClearSelection();

	private:

		static constexpr USize32 MaxShownSystems = 15;

		DynamicArray<OSK::UI::Button*> m_textViews{};
		EditorPanelTitle* m_title = nullptr;
		PropertiesPanel* m_propertiesPanel = nullptr;

	};

}
