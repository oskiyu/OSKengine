#include "ColliderRenderSystemController.h"

#include "ColliderRenderSystemView.h"

OSK::Editor::Controllers::ColliderRenderSystemController::ColliderRenderSystemController(ECS::ISystem* system, Views::ISystemView* view)
	: TSystemController<ECS::ColliderRenderSystem>(system, view)
{
	view->As<Views::ColliderRenderSystemView>()->SetCameraObject(GetSystem()->GetCurrentCameraObject());
	view->As<Views::ColliderRenderSystemView>()->SetTopLevelMaterialName(GetSystem()->GetTopLevelMaterial()->GetName());
	view->As<Views::ColliderRenderSystemView>()->SetBottomLevelMaterialName(GetSystem()->GetBottomLevelMaterial()->GetName());
	view->As<Views::ColliderRenderSystemView>()->SetPointsMaterialName(GetSystem()->GetPointsMaterial()->GetName());
}

void OSK::Editor::Controllers::ColliderRenderSystemController::Poll() {
	const auto currentCameraObject = GetSystem()->GetCurrentCameraObject();
	if (m_previousCameraObject != currentCameraObject) {
		GetView()->As<Views::ColliderRenderSystemView>()->SetCameraObject(currentCameraObject);
		m_previousCameraObject = currentCameraObject;
	}

	const auto currentTopMaterial = GetSystem()->GetTopLevelMaterial()->GetName();
	if (m_previousMainMaterialName != currentTopMaterial) {
		GetView()->As<Views::ColliderRenderSystemView>()->SetTopLevelMaterialName(currentTopMaterial);
		m_previousMainMaterialName = currentTopMaterial;
	}

	const auto currentBottomMaterialName = GetSystem()->GetBottomLevelMaterial()->GetName();
	if (m_previousLowLevelMaterialName != currentBottomMaterialName) {
		GetView()->As<Views::ColliderRenderSystemView>()->SetBottomLevelMaterialName(currentBottomMaterialName);
		m_previousLowLevelMaterialName = currentBottomMaterialName;
	}

	const auto currentPointMaterialName = GetSystem()->GetPointsMaterial()->GetName();
	if (m_previousPointMaterialName != currentPointMaterialName) {
		GetView()->As<Views::ColliderRenderSystemView>()->SetPointsMaterialName(currentPointMaterialName);
		m_previousPointMaterialName = currentPointMaterialName;
	}
}
