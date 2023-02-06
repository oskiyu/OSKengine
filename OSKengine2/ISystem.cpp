#include "ISystem.h"

using namespace OSK::ECS;

void ISystem::OnCreate() {

}

void ISystem::OnTick(TDeltaTime deltaTime) {

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
	this->isActive = isActive;
}

void ISystem::ToggleActivationStatus() {
	SetActivationStatus(!isActive);
}

bool ISystem::IsActive() const {
	return isActive;
}
