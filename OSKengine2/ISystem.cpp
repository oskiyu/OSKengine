#include "ISystem.h"

using namespace OSK::ECS;

void ISystem::OnCreate() {

}

void ISystem::OnTick(TDeltaTime deltaTime) {

}

void ISystem::OnRemove() {

}

void ISystem::AddObject(GameObjectIndex obj) {
	processedObjects.Insert(obj);
}

void ISystem::RemoveObject(GameObjectIndex obj) {
	processedObjects.Remove(obj);
}

const Signature& ISystem::GetSignature() const {
	return signature;
}
