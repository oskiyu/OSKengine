#include "GameObjectManager.h"

#include "Component.h"

using namespace OSK::ECS;

GameObjectManager::GameObjectManager() {
	for (GameObjectID obj = 0; obj < MAX_OBJECTS; obj++)
		freeObjects.push(obj);
}

GameObjectID GameObjectManager::CreateGameObject() {
	livingEnitites++;

	GameObjectID ID = freeObjects.front();

	freeObjects.pop();

	return ID;
}

void GameObjectManager::DestroyGameObject(GameObjectID object) {
	livingEnitites--;

	signatures[object].reset();

	freeObjects.push(object);
}

void GameObjectManager::SetSignature(GameObjectID obj, Signature sign) {
	signatures[obj] = sign;
}

OSK::Signature GameObjectManager::GetSignature(GameObjectID obj) const {
	return signatures[obj];
}
