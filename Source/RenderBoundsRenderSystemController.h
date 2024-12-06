#pragma once

#include "ISystemController.h"
#include "RenderBoundsRenderer.h"
#include "GameObject.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL RenderBoundsRenderSystemController : public TSystemController<ECS::RenderBoundsRenderer> {

	public:

		OSK_LINK_SYSTEM_EDITOR_CONTROLLER(ECS::RenderBoundsRenderer);

		explicit RenderBoundsRenderSystemController(ECS::ISystem* system, Views::ISystemView* view);

		void Poll() override;

	private:

		ECS::GameObjectIndex m_previousCameraObject = ECS::GameObjectIndex::CreateEmpty();
		std::string m_previousMaterialName{};

	};

}
