#pragma once

#include "IComponentView.h"
#include "CameraComponent3D.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL CameraComponentView : public IComponentView {

	public:

		OSK_LINK_COMPONENT_EDITOR_VIEW(ECS::CameraComponent3D);

		explicit CameraComponentView(const Vector2f& size);

		OSK::UI::TextView* GetFovView();

	private:

		OSK::UI::TextView* m_fovView = nullptr;

	};

}
