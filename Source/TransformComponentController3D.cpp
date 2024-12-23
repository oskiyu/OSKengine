#include "TransformComponentController3D.h"

#include "TransformComponentView3D.h"

#include "UiTextView.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::Editor;

Controllers::TransformComponentController3D::TransformComponentController3D(GameObjectIndex object, void* componentData, Views::IComponentView* view)
	: Controllers::TComponentController<ECS::TransformComponent3D>(object, componentData, view) {

}

void Controllers::TransformComponentController3D::Poll() {
	const TransformComponent3D& transform = *GetComponent();
	Views::TransformComponentView3D* view = GetView()->As<Views::TransformComponentView3D>();

	if (m_previousPosition != transform.GetTransform().GetPosition()) {
		view->SetPosition(transform.GetTransform().GetPosition());
		m_previousPosition = transform.GetTransform().GetPosition();
	}

	const Vector3f eulerRotation = Vector3f(glm::degrees(transform.GetTransform().GetRotation().ToEulerAngles().ToGlm()));

	if (m_previousRotation != eulerRotation) {
		view->SetRotation(eulerRotation);
		m_previousRotation = eulerRotation;
	}

	if (m_previousScale != transform.GetTransform().GetScale()) {
		view->SetScale(transform.GetTransform().GetScale());
		m_previousScale = transform.GetTransform().GetScale();
	}
}
