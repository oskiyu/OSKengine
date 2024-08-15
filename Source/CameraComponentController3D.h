#pragma once

#include "IComponentController.h"
#include "CameraComponent3D.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL CameraComponentController3D : public TComponentController<ECS::CameraComponent3D> {

	public:

		OSK_LINK_COMPONENT_EDITOR_CONTROLLER(ECS::CameraComponent3D);

		CameraComponentController3D(
			ECS::GameObjectIndex object,
			void* componentData,
			Views::IComponentView* view);

	private:

		void Poll() override;

		float m_previousFov = 0.0f;

	};

}
