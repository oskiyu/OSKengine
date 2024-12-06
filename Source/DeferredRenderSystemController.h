#pragma once

#include "ISystemController.h"
#include "DeferredRenderSystem.h"
#include "GameObject.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL DeferredRenderSystemController : public TSystemController<ECS::DeferredRenderSystem> {

	public:

		OSK_LINK_SYSTEM_EDITOR_CONTROLLER(ECS::DeferredRenderSystem);

		explicit DeferredRenderSystemController(ECS::ISystem* system, Views::ISystemView* view);

		void Poll() override;

	private:

		std::string m_previousIrradianceMapName{};
		std::string m_previousSpecularMapName{};
		std::string m_previousResolverName{};
		ECS::GameObjectIndex m_previousCameraObject = ECS::GameObjectIndex::CreateEmpty();

	};

}
