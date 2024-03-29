#include "GameObjectManager.h"

#include "Component.h"
#include "Assert.h"

#include "EcsExceptions.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;

GameObjectIndex GameObjectManager::CreateGameObject() {
	if (!m_freeObjectIndices.IsEmpty()) {
		m_signatures[m_freeObjectIndices.Peek()] = {};
		return m_freeObjectIndices.Pop();
	}

	GameObjectIndex output = m_nextIndex;
	m_signatures[output] = {};

	m_nextIndex++;

	Engine::GetLogger()->InfoLog(std::format("New object: {}", output));

	return output;
}

void GameObjectManager::DestroyGameObject(GameObjectIndex* obj) {
	OSK_ASSERT(*obj > 0, InvalidObjectException(*obj));
	OSK_ASSERT(m_signatures.contains(*obj), InvalidObjectException(*obj));

	m_signatures.erase(*obj);
	m_freeObjectIndices.Push(*obj);

	Engine::GetLogger()->InfoLog(std::format("Destroyed object: {}", *obj));

	*obj = 0;
}

void GameObjectManager::SetSignature(GameObjectIndex obj, const Signature& signature) {
	OSK_ASSERT(obj > 0, InvalidObjectException(obj));
	OSK_ASSERT(m_signatures.contains(obj), InvalidObjectException(obj));

	m_signatures[obj] = signature;
}

Signature GameObjectManager::GetSignature(GameObjectIndex obj) const {
	OSK_ASSERT(obj > 0, InvalidObjectException(obj));
	OSK_ASSERT(m_signatures.contains(obj), InvalidObjectException(obj));

	return m_signatures.at(obj);
}

bool GameObjectManager::IsGameObjectAlive(GameObjectIndex obj) const {
	if (obj >= m_nextIndex)
		return true;

	if (m_freeObjectIndices.ContainsElement(obj))
		return true;

	return false;
}
