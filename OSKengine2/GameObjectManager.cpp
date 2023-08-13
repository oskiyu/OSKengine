#include "GameObjectManager.h"

#include "Component.h"
#include "Assert.h"

#include "EcsExceptions.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;

GameObjectIndex GameObjectManager::CreateGameObject() {
	if (!freeObjectIndices.IsEmpty())
		return freeObjectIndices.Pop();

	GameObjectIndex output = nextIndex;
	signatures.Insert({});

	nextIndex++;

	Engine::GetLogger()->InfoLog(std::format("New object: {}", output));

	return output;
}

void GameObjectManager::DestroyGameObject(GameObjectIndex* obj) {
	OSK_ASSERT(*obj > 0, InvalidObjectException(*obj));
	OSK_ASSERT(*obj < signatures.GetSize(), InvalidObjectException(*obj));

	signatures[(*obj) - 1].Reset();
	freeObjectIndices.Push(*obj);

	Engine::GetLogger()->InfoLog(std::format("Destroyed object: {}", *obj));

	*obj = 0;
}

void GameObjectManager::SetSignature(GameObjectIndex obj, const Signature& signature) {
	OSK_ASSERT(obj > 0, InvalidObjectException(obj));
	OSK_ASSERT(obj < signatures.GetSize() + 1, InvalidObjectException(obj));
	signatures[obj - 1] = signature;
}

Signature GameObjectManager::GetSignature(GameObjectIndex obj) const {
	OSK_ASSERT(obj > 0, InvalidObjectException(obj));
	OSK_ASSERT(obj < signatures.GetSize() + 1, InvalidObjectException(obj));
	return signatures[obj - 1];
}

bool GameObjectManager::IsGameObjectAlive(GameObjectIndex obj) const {
	if (obj >= nextIndex)
		return true;

	if (freeObjectIndices.ContainsElement(obj))
		return true;

	return false;
}
