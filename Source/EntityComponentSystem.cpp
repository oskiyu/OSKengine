#include "EntityComponentSystem.h"

#include "SystemManager.h"
#include "ComponentManager.h"
#include "GameObjectManager.h"

using namespace OSK;
using namespace OSK::ECS;

EntityComponentSystem::EntityComponentSystem(IO::ILogger* logger) : logger(logger){
	systemManager = new SystemManager;
	componentManager = new ComponentManager;
	gameObjectManager = new GameObjectManager;
	eventManager = new EventManager;
}

void EntityComponentSystem::OnTick(TDeltaTime deltaTime) {
	systemManager->OnTick(deltaTime);
}

void EntityComponentSystem::_ClearEventQueues() {
	eventManager->_ClearQueues();
}

GameObjectIndex EntityComponentSystem::SpawnObject() {
	return gameObjectManager->CreateGameObject();
}

void EntityComponentSystem::DestroyObject(GameObjectIndex* obj) {
	gameObjectManager->DestroyGameObject(obj);
}

bool EntityComponentSystem::IsGameObjectAlive(GameObjectIndex obj) const {
	return gameObjectManager->IsGameObjectAlive(obj);
}

void EntityComponentSystem::EndFrame() {
	eventManager->_ClearQueues();
}

const DynamicArray<IRenderSystem*>& EntityComponentSystem::GetRenderSystems() const {
	return renderSystems;
}
