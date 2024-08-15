#include "CameraComponentController3D.h"

#include "CameraComponentView.h"

#include "UiTextView.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::Editor;

Controllers::CameraComponentController3D::CameraComponentController3D(GameObjectIndex object, void* componentData, Views::IComponentView* view)
	: Controllers::TComponentController<ECS::CameraComponent3D>(object, componentData, view) {

}

void Controllers::CameraComponentController3D::Poll() {
	const CameraComponent3D& camera = *GetComponent();
	Views::CameraComponentView* view = GetView()->As<Views::CameraComponentView>();

	if (m_previousFov != camera.GetFov()) {
		auto fovView = view->GetFovView();
		fovView->SetText(std::format("FOV: {}", camera.GetFov()));
		m_previousFov = camera.GetFov();
	}
}
