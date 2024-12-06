#pragma once

#include "ISystemView.h"
#include "Vector3.hpp"

#include "SkyboxRenderSystem.h"
#include "GameObject.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL SkyboxRenderSystemView : public ISystemView {

	public:

		OSK_LINK_SYSTEM_EDITOR_VIEW(ECS::SkyboxRenderSystem);

		explicit SkyboxRenderSystemView(const Vector2f& size);

		void SetTextureName(const std::string& name);
		void SetMaterialName(const std::string& name);
		void SetCameraObject(ECS::GameObjectIndex object);

	private:

		OSK::UI::TextView* m_textureNameView = nullptr;
		OSK::UI::TextView* m_materialNameView = nullptr;
		OSK::UI::TextView* m_cameraObjectView = nullptr;

	};

}
