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
	m_livingObjects.Insert(output);

	m_nextIndex++;

	return output;
}

void GameObjectManager::DestroyGameObject(GameObjectIndex* obj) {
	OSK_ASSERT(obj->Get() > 0, InvalidObjectException(*obj));
	OSK_ASSERT(m_signatures.contains(*obj), InvalidObjectException(*obj));

	m_signatures.erase(*obj);
	m_freeObjectIndices.Push(*obj);
	m_livingObjects.Remove(*obj);

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

std::span<const GameObjectIndex> GameObjectManager::GetAllLivingObjects() const {
	return m_livingObjects.GetFullSpan();
}

void GameObjectManager::UpdateSignature(GameObjectIndex obj, ComponentType position, bool value) {
	m_signatures.at(obj).SetValue(position, value);
}

void GameObjectManager::AddComponent(GameObjectIndex obj, ComponentType position) {
	UpdateSignature(obj, position, true);
}

void GameObjectManager::RemoveComponent(GameObjectIndex obj, ComponentType position) {
	UpdateSignature(obj, position, false);
}