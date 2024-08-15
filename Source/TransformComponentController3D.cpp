#include "TransformComponentController3D.h"

#include "TransformComponentView3D.h"

#include "UiTextView.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::Editor;

Controllers::TransformComponentController3D::TransformComponentController3D(GameObjectIndex object, void* componentData, Views::IComponentView* view)
	: Controllers::TComponentController<ECS::Transform3D>(object, componentData, view) {

}

void Controllers::TransformComponentController3D::Poll() {
	const Transform3D& transform = *GetComponent();
	Views::TransformComponentView3D* view = GetView()->As<Views::TransformComponentView3D>();

	if (m_previousPosition != transform.GetPosition()) {
		view->SetPosition(transform.GetPosition());
		m_previousPosition = transform.GetPosition();
	}

	const Vector3f eulerRotation = Vector3f(glm::degrees(transform.GetRotation().ToEulerAngles().ToGlm()));

	if (m_previousRotation != eulerRotation) {
		view->SetRotation(eulerRotation);
		m_previousRotation = eulerRotation;
	}

	if (m_previousScale != transform.GetScale()) {
		view->SetScale(transform.GetScale());
		m_previousScale = transform.GetScale();
	}
}
