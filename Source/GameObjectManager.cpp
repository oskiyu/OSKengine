#include "GameObjectManager.h"

#include "Component.h"
#include "Assert.h"

#include "EcsExceptions.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;

GameObjectIndex GameObjectManager::CreateGameObject() {
	// if (!m_freeObjectIndices.IsEmpty()) {
	//	m_signatures[m_freeObjectIndices.Peek()] = {};
	//	return m_freeObjectIndices.Pop();
	//}

	GameObjectIndex output = GameObjectIndex(m_nextIndex);
	m_signatures[output] = {};

	m_nextIndex++;

	return output;
}

void GameObjectManager::DestroyGameObject(GameObjectIndex* obj) {
	OSK_ASSERT(obj->Get() > 0, InvalidObjectException(*obj));
	OSK_ASSERT(m_signatures.contains(*obj), InvalidObjectException(*obj));

	m_signatures.erase(*obj);
	m_freeObjectIndices.Push(*obj);

	obj->SetValue(0);
}

void GameObjectManager::SetSignature(GameObjectIndex obj, const Signature& signature) {
	OSK_ASSERT(obj.Get() > 0, InvalidObjectException(obj));
	OSK_ASSERT(m_signatures.contains(obj), InvalidObjectException(obj));

	m_signatures[obj] = signature;
}

Signature GameObjectManager::GetSignature(GameObjectIndex obj) const {
	OSK_ASSERT(obj.Get() > 0, InvalidObjectException(obj));
	OSK_ASSERT(m_signatures.contains(obj), InvalidObjectException(obj));

	return m_signatures.at(obj);
}

bool GameObjectManager::IsGameObjectAlive(GameObjectIndex obj) const {
	return m_signatures.contains(obj);
}
