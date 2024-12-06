#include "SkyboxRenderSystemController.h"

#include "SkyboxRenderSystemView.h"

OSK::Editor::Controllers::SkyboxRenderSystemController::SkyboxRenderSystemController(ECS::ISystem* system, Views::ISystemView* view)
	: TSystemController<ECS::SkyboxRenderSystem>(system, view)
{
	view->As<Views::SkyboxRenderSystemView>()->SetTextureName(GetSystem()->GetCurrentTexture()->GetName());
	view->As<Views::SkyboxRenderSystemView>()->SetCameraObject(GetSystem()->GetCurrentCameraObject());
	view->As<Views::SkyboxRenderSystemView>()->SetMaterialName(static_cast<std::string>(GetSystem()->GetCurrentMaterial()->GetName()));
}

void OSK::Editor::Controllers::SkyboxRenderSystemController::Poll() {
	const auto currentTextureName = GetSystem()->GetCurrentTexture()->GetName();
	if (m_previousTextureName != currentTextureName) {
		GetView()->As<Views::SkyboxRenderSystemView>()->SetTextureName(GetSystem()->GetCurrentTexture()->GetName());
		m_previousTextureName = currentTextureName;
	}

	const auto currentCameraObject = GetSystem()->GetCurrentCameraObject();
	if (m_previousCameraObject != currentCameraObject) {
		GetView()->As<Views::SkyboxRenderSystemView>()->SetCameraObject(currentCameraObject);
		m_previousCameraObject = currentCameraObject;
	}

	const auto currentMaterialName = GetSystem()->GetCurrentMaterial()->GetName();
	if (m_previousMaterialName != currentMaterialName) {
		GetView()->As<Views::SkyboxRenderSystemView>()->SetMaterialName(static_cast<std::string>(currentMaterialName));
		m_previousMaterialName = currentMaterialName;
	}
}
