#include "DeferredRenderSystemController.h"

#include "DeferredRenderSystemView.h"

OSK::Editor::Controllers::DeferredRenderSystemController::DeferredRenderSystemController(ECS::ISystem* system, Views::ISystemView* view)
	: TSystemController<ECS::DeferredRenderSystem>(system, view)
{
	view->As<Views::DeferredRenderSystemView>()->SetIrradianceMapName(GetSystem()->GetIrradianceMap()->GetName());
	view->As<Views::DeferredRenderSystemView>()->SetSpecularMapName(GetSystem()->GetSpecularMap()->GetName());
	view->As<Views::DeferredRenderSystemView>()->SetCameraObject(GetSystem()->GetCurrentCameraObject());
}

void OSK::Editor::Controllers::DeferredRenderSystemController::Poll() {
	const auto currentIrradianceName = GetSystem()->GetIrradianceMap()->GetName();
	if (m_previousIrradianceMapName != currentIrradianceName) {
		GetView()->As<Views::DeferredRenderSystemView>()->SetIrradianceMapName(currentIrradianceName);
		m_previousIrradianceMapName = currentIrradianceName;
	}

	const auto currentSpecularName = GetSystem()->GetSpecularMap()->GetName();
	if (m_previousSpecularMapName != currentSpecularName) {
		GetView()->As<Views::DeferredRenderSystemView>()->SetSpecularMapName(currentSpecularName);
		m_previousSpecularMapName = currentSpecularName;
	}

	const auto currentCameraObject = GetSystem()->GetCurrentCameraObject();
	if (m_previousCameraObject != currentCameraObject) {
		GetView()->As<Views::DeferredRenderSystemView>()->SetCameraObject(currentCameraObject);
		m_previousCameraObject = currentCameraObject;
	}

	const auto currentResolverName = GetSystem()->GetResolverPassName();
	if (m_previousResolverName != currentResolverName) {
		GetView()->As<Views::DeferredRenderSystemView>()->SetResolverPassName(currentResolverName);
		m_previousResolverName = currentResolverName;
	}

	for (const auto& pass : GetSystem()->GetAllShaderPasses()) {
		GetView()->As<Views::DeferredRenderSystemView>()->AddShaderPass(static_cast<std::string>(pass->GetTypeName()));
	}

	for (const auto& pass : GetSystem()->GetAllShadowsPasses()) {
		GetView()->As<Views::DeferredRenderSystemView>()->AddShadowsPass(static_cast<std::string>(pass->GetTypeName()));
	}
}
