#pragma once

#include "ISystemController.h"
#include "SkyboxRenderSystem.h"
#include "GameObject.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL SkyboxRenderSystemController : public TSystemController<ECS::SkyboxRenderSystem> {

	public:

		OSK_LINK_SYSTEM_EDITOR_CONTROLLER(ECS::SkyboxRenderSystem);

		explicit SkyboxRenderSystemController(ECS::ISystem* system, Views::ISystemView* view);

		void Poll() override;

	private:

		std::string m_previousTextureName{};
		std::string m_previousMaterialName{};
		ECS::GameObjectIndex m_previousCameraObject = ECS::GameObjectIndex::CreateEmpty();

	};

}
