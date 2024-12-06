#pragma once

#include "ISystemView.h"
#include "Vector3.hpp"

#include "ColliderRenderer.h"
#include "GameObject.h"
#include "DynamicArray.hpp"

namespace OSK::UI {
	class TextView;
	class CollapsibleWrapper;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL ColliderRenderSystemView : public ISystemView {

	public:

		OSK_LINK_SYSTEM_EDITOR_VIEW(ECS::ColliderRenderSystem);

		explicit ColliderRenderSystemView(const Vector2f& size);

		void SetCameraObject(ECS::GameObjectIndex object);
		void SetTopLevelMaterialName(std::string_view name);
		void SetBottomLevelMaterialName(std::string_view name);
		void SetPointsMaterialName(std::string_view name);

	private:

		OSK::UI::TextView* m_cameraObjectView = nullptr;
		OSK::UI::TextView* m_topLevelMaterialNameView = nullptr;
		OSK::UI::TextView* m_bottomLevelMaterialNameView = nullptr;
		OSK::UI::TextView* m_pointsMaterialNameView = nullptr;
	};

}
