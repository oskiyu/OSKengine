#include "IIteratorSystem.h"

using namespace OSK;
using namespace OSK::ECS;

void IIteratorSystem::OnObjectAdded(GameObjectIndex obj) {

}

void IIteratorSystem::OnObjectRemoved(GameObjectIndex obj) {

}

const Signature& IIteratorSystem::GetSignature() const {
	return signature;
}

void IIteratorSystem::_AddObject(GameObjectIndex obj) {
	processedObjects.Insert(obj);

	OnObjectAdded(obj);
}

void IIteratorSystem::_RemoveObject(GameObjectIndex obj) {
	processedObjects.Remove(obj);

	OnObjectRemoved(obj);
}

void IIteratorSystem::_SetSignature(const Signature& signature) {
	this->signature = signature;
}

const DynamicArray<GameObjectIndex>& IIteratorSystem::GetObjects() {
	return processedObjects;
}
