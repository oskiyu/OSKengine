#include "IIteratorSystem.h"

using namespace OSK;
using namespace OSK::ECS;

void IIteratorSystem::OnObjectAdded(GameObjectIndex obj) {

}

void IIteratorSystem::OnObjectRemoved(GameObjectIndex obj) {

}

void IIteratorSystem::OnExecutionStart() {

}

void IIteratorSystem::Execute(TDeltaTime deltaTime, std::span<const ECS::GameObjectIndex> objects) {

}

void IIteratorSystem::OnExecutionEnd() {

}

const Signature& IIteratorSystem::GetSignature() const {
	return m_signature;
}

void IIteratorSystem::_SetSignature(const Signature& signature) {
	m_signature = signature;
}

void IIteratorSystem::SetCompatibleObjects(std::span<const ECS::GameObjectIndex> objects) {
	m_allCompatibleObjects = objects;
}

std::span<const ECS::GameObjectIndex> IIteratorSystem::GetAllCompatibleObjects() const {
	return m_allCompatibleObjects;
}
