#pragma once

#include "ISystemController.h"
#include "ColliderRenderer.h"
#include "GameObject.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL ColliderRenderSystemController : public TSystemController<ECS::ColliderRenderSystem> {

	public:

		OSK_LINK_SYSTEM_EDITOR_CONTROLLER(ECS::ColliderRenderSystem);

		explicit ColliderRenderSystemController(ECS::ISystem* system, Views::ISystemView* view);

		void Poll() override;

	private:

		ECS::GameObjectIndex m_previousCameraObject = ECS::GameObjectIndex::CreateEmpty();
		std::string m_previousMainMaterialName{};
		std::string m_previousLowLevelMaterialName{};
		std::string m_previousPointMaterialName{};

	};

}
