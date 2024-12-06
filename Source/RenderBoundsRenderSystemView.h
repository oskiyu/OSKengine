#pragma once

#include "ISystemView.h"
#include "Vector3.hpp"

#include "RenderBoundsRenderer.h"
#include "GameObject.h"
#include "DynamicArray.hpp"

namespace OSK::UI {
	class TextView;
	class CollapsibleWrapper;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL RenderBoundsRenderSystemView : public ISystemView {

	public:

		OSK_LINK_SYSTEM_EDITOR_VIEW(ECS::RenderBoundsRenderer);

		explicit RenderBoundsRenderSystemView(const Vector2f& size);

		void SetCameraObject(ECS::GameObjectIndex object);
		void SetMaterialName(std::string_view name);

	private:

		OSK::UI::TextView* m_cameraObjectView = nullptr;
		OSK::UI::TextView* m_materialNameView = nullptr;

	};

}
