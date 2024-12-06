#pragma once

#include "ApiCall.h"
#include "UiVerticalContainer.h"

#include <type_traits>

namespace OSK::UI {
	class TextView;
	class Button;
}

namespace OSK::Editor::Views {

	/// @brief Elemento de interfaz de usuario que muestra las
	/// propiedades de un componente en el editor.
	class OSKAPI_CALL ISystemView : public OSK::UI::VerticalContainer {

	public:

		virtual std::string_view _GetSystemName() const = 0;

		void SetInitialSystemState(bool isActive);

	protected:

		explicit ISystemView(
			const Vector2f& size,
			const std::string& title);

		void OnSizeChanged(const Vector2f& previousSize) override;

	private:

		UIndex32 m_backgroundDrawCallIndex = 0;
		OSK::UI::TextView* m_titleView = nullptr;

	};

	template <typename _TSystemView>
	concept IsSystemView =
		std::is_base_of_v<ISystemView, _TSystemView>&&
		requires (_TSystemView) {
			{ _TSystemView::GetSystemName() };
	};

}

#define OSK_SYSTEM_EDITOR_VIEW(systemName) \
constexpr static std::string_view GetSystemName() { return systemName; } \
std::string_view _GetSystemName() const override { return systemName; }

#define OSK_LINK_SYSTEM_EDITOR_VIEW(componentType) \
constexpr static std::string_view GetSystemName() { return componentType::GetSystemName(); } \
std::string_view _GetSystemName() const override { return componentType::GetSystemName(); }
