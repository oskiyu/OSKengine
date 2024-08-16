#pragma once

#include "ApiCall.h"
#include "UiVerticalContainer.h"
#include "UiCollapsibleWrapper.h"

#include <type_traits>

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	/// @brief Elemento de interfaz de usuario que muestra las
	/// propiedades de un componente en el editor.
	class OSKAPI_CALL IComponentView : public OSK::UI::CollapsibleWrapper {

	public:

	protected:

		IComponentView(
			const Vector2f& size,
			const std::string& title);

		void OnSizeChanged(const Vector2f& previousSize) override;

		/// @return Contenedor en el que se inserta
		/// la información del componente.
		VerticalContainer* GetUnderlyingContainer();

	private:

		UIndex32 m_backgroundDrawCallIndex = 0;
		OSK::UI::TextView* m_titleView = nullptr;

	};

	template <typename _TComponentController>
	concept IsComponentView =
		std::is_base_of_v<IComponentView, _TComponentController> &&
		requires (_TComponentController) {
			{ _TComponentController::GetComponentTypeName() };
	};

}

#define OSK_COMPONENT_EDITOR_VIEW(componentName) \
constexpr static std::string_view GetComponentTypeName() { return componentName; }

#define OSK_LINK_COMPONENT_EDITOR_VIEW(componentType) \
constexpr static std::string_view GetComponentTypeName() { return componentType::GetComponentTypeName(); }
