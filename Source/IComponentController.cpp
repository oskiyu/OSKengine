#include "IComponentController.h"

using namespace OSK;
using namespace OSK::Editor::Controllers;
using namespace OSK::Editor::Views;

void IComponentController::Update(void* component) {
	m_componentData = component;
}

OSK::ECS::GameObjectIndex IComponentController::GetLinkedObject() const {
	return m_linkedObject;
}

OSK::ECS::ComponentType IComponentController::GetLinkedComponentType() const {
	return m_linkedComponent;
}

IComponentController::IComponentController(ECS::GameObjectIndex object, void* componentData, IComponentView* view) : m_linkedObject(object), m_view(view), m_componentData(componentData) {

}

IComponentView* IComponentController::GetView() {
	return m_view;
}

const IComponentView* IComponentController::GetView() const {
	return m_view;
}

void* IComponentController::GetComponentData() {
	return m_componentData;
}

const void* IComponentController::GetComponentData() const {
	return m_componentData;
}
