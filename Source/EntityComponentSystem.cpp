#include "EntityComponentSystem.h"

#include "SystemManager.h"
#include "ComponentManager.h"
#include "GameObjectManager.h"

using namespace OSK;
using namespace OSK::ECS;

EntityComponentSystem::EntityComponentSystem(IO::ILogger* logger) : m_logger(logger) {

}

void EntityComponentSystem::OnTick(TDeltaTime deltaTime) {
	m_systemManager->OnTick(deltaTime, m_eventManager.GetValue());
}

void EntityComponentSystem::OnRender(GRAPHICS::ICommandList* commandList) {
	m_systemManager->OnRender(commandList);
}

void EntityComponentSystem::_ClearEventQueues() {
	m_eventManager->_ClearQueues();
}

GameObjectIndex EntityComponentSystem::SpawnObject() {
	return m_gameObjectManager->CreateGameObject();
}

void EntityComponentSystem::DestroyObject(GameObjectIndex* obj) {
	m_systemManager->GameObjectDestroyed(*obj);
	m_gameObjectManager->DestroyGameObject(obj);
}

bool EntityComponentSystem::IsGameObjectAlive(GameObjectIndex obj) const {
	return m_gameObjectManager->IsGameObjectAlive(obj);
}

void EntityComponentSystem::EndFrame() {
	m_eventManager->_ClearQueues();
}

const DynamicArray<IRenderSystem*>& EntityComponentSystem::GetRenderSystems() const {
	return m_renderSystems;
}
