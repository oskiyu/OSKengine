#include "ISystemController.h"

#include "ISystemView.h"

OSK::Editor::Controllers::ISystemController::ISystemController(ECS::ISystem* systemData, Views::ISystemView* view) : m_system(systemData), m_view(view) {
	
}

OSK::Editor::Views::ISystemView* OSK::Editor::Controllers::ISystemController::GetView() {
	return m_view;
}

const OSK::Editor::Views::ISystemView* OSK::Editor::Controllers::ISystemController::GetView() const {
	return m_view;
}

OSK::ECS::ISystem* OSK::Editor::Controllers::ISystemController::GetSystemData() {
	return m_system;
}

const OSK::ECS::ISystem* OSK::Editor::Controllers::ISystemController::GetSystemData() const {
	return m_system;
}
