#include "GameObjectManager.h"

#include "Component.h"
#include "Assert.h"

using namespace OSK;
using namespace OSK::ECS;

GameObjectIndex GameObjectManager::CreateGameObject() {
	if (!freeObjectIndices.IsEmpty())
		return freeObjectIndices.Pop();

	GameObjectIndex output = nextIndex;
	signatures.Insert({});

	nextIndex++;

	return output;
}

void GameObjectManager::DestroyGameObject(GameObjectIndex* obj) {
	OSK_ASSERT(*obj > 0, "Se ha intentado acceder a un objeto inv�lido.");

	signatures[*obj - 1].Reset();
	freeObjectIndices.Push(*obj);

	*obj = 0;
}

void GameObjectManager::SetSignature(GameObjectIndex obj, const Signature& signature) {
	OSK_ASSERT(obj > 0, "Se ha intentado acceder a un objeto inv�lido.");
	signatures[obj - 1] = signature;
}

Signature GameObjectManager::GetSignature(GameObjectIndex obj) {
	OSK_ASSERT(obj > 0, "Se ha intentado acceder a un objeto inv�lido.");
	return signatures[obj - 1];
}

bool GameObjectManager::IsGameObjectAlive(GameObjectIndex obj) const {
	if (obj >= nextIndex)
		return true;

	if (freeObjectIndices.ContainsElement(obj))
		return true;

	return false;
}
