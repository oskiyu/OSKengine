#include "ISystem.h"

using namespace OSK::ECS;

void ISystem::OnCreate() {

}

void ISystem::OnTick(TDeltaTime deltaTime) {

}

void ISystem::OnRemove() {

}

void ISystem::OnObjectAdded(GameObjectIndex obj) {

}

void ISystem::OnObjectRemoved(GameObjectIndex obj) {

}

void ISystem::AddObject(GameObjectIndex obj) {
	processedObjects.Insert(obj);

	OnObjectAdded(obj);
}

void ISystem::RemoveObject(GameObjectIndex obj) {
	processedObjects.Remove(obj);

	OnObjectRemoved(obj);
}

void ISystem::SetSignature(const Signature& signature) {
	this->signature = signature;
}

const OSK::DynamicArray<GameObjectIndex>& ISystem::GetObjects() {
	return processedObjects;
}

const Signature& ISystem::GetSignature() const {
	return signature;
}
