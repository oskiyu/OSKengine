#pragma once

#include "IComponentController.h"
#include "ModelComponent3D.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL ModelComponentController3D : public TComponentController<ECS::ModelComponent3D> {

	public:

		OSK_LINK_COMPONENT_EDITOR_CONTROLLER(ECS::ModelComponent3D);

		ModelComponentController3D(
			ECS::GameObjectIndex object,
			void* componentData,
			Views::IComponentView* view);

	private:

		void Poll() override;

		bool m_isFirstUpdate = true;

		std::string m_previousAssetPath = "";
		bool m_castedShadows = false;
		bool m_wasAnimated = false;

	};

}
