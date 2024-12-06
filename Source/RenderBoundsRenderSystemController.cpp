#include "RenderBoundsRenderSystemController.h"

#include "RenderBoundsRenderSystemView.h"

OSK::Editor::Controllers::RenderBoundsRenderSystemController::RenderBoundsRenderSystemController(ECS::ISystem* system, Views::ISystemView* view)
	: TSystemController<ECS::RenderBoundsRenderer>(system, view)
{
	view->As<Views::RenderBoundsRenderSystemView>()->SetCameraObject(GetSystem()->GetCurrentCameraObject());
	view->As<Views::RenderBoundsRenderSystemView>()->SetMaterialName(GetSystem()->GetMaterial()->GetName());
}

void OSK::Editor::Controllers::RenderBoundsRenderSystemController::Poll() {
	const auto currentCameraObject = GetSystem()->GetCurrentCameraObject();
	if (m_previousCameraObject != currentCameraObject) {
		GetView()->As<Views::RenderBoundsRenderSystemView>()->SetCameraObject(currentCameraObject);
		m_previousCameraObject = currentCameraObject;
	}

	const auto currentMaterialName = GetSystem()->GetMaterial()->GetName();
	if (m_previousMaterialName != currentMaterialName) {
		GetView()->As<Views::RenderBoundsRenderSystemView>()->SetMaterialName(currentMaterialName);
		m_previousMaterialName = currentMaterialName;
	}
}
