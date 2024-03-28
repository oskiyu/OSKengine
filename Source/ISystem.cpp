#include "ISystem.h"

using namespace OSK::ECS;

void ISystem::OnCreate() {

}

void ISystem::OnRemove() {

}

void ISystem::Activate() {
	SetActivationStatus(true);
}

void ISystem::Deactivate() {
	SetActivationStatus(false);
}

void ISystem::SetActivationStatus(bool isActive) {
	m_isActive = isActive;
}

void ISystem::ToggleActivationStatus() {
	SetActivationStatus(!m_isActive);
}

bool ISystem::IsActive() const {
	return m_isActive;
}

void ISystem::_SetDependencies(const SystemDependencies& dependencies) {
	m_dependencies = dependencies;
}

const SystemDependencies& ISystem::GetDependencies() const {
	return m_dependencies;
}
