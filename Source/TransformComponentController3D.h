#pragma once

#include "IComponentController.h"
#include "TransformComponent3D.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL TransformComponentController3D : public TComponentController<ECS::TransformComponent3D> {

	public:

		OSK_LINK_COMPONENT_EDITOR_CONTROLLER(ECS::TransformComponent3D);

		TransformComponentController3D(
			ECS::GameObjectIndex object,
			void* componentData,
			Views::IComponentView* view);

	private:

		void Poll() override;

		Vector3f m_previousPosition = Vector3f::Zero;
		Vector3f m_previousRotation = Vector3f::Zero;
		Vector3f m_previousScale = Vector3f::Zero;

	};

}
