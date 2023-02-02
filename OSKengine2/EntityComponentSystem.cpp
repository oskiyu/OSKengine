#include "EntityComponentSystem.h"

#include "SystemManager.h"
#include "ComponentManager.h"
#include "GameObjectManager.h"

using namespace OSK;
using namespace OSK::ECS;

EntityComponentSystem::EntityComponentSystem() {
	systemManager = new SystemManager;
	componentManager = new ComponentManager;
	gameObjectManager = new GameObjectManager;
	eventManager = new EventManager;
}

void EntityComponentSystem::OnTick(TDeltaTime deltaTime) {
	systemManager->OnTick(deltaTime);
}

GameObjectIndex EntityComponentSystem::SpawnObject() {
	return gameObjectManager->CreateGameObject();
}

void EntityComponentSystem::DestroyObject(GameObjectIndex* obj) {
	gameObjectManager->DestroyGameObject(obj);
}

void EntityComponentSystem::EndFrame() {
	eventManager->_ClearQueues();
}

const DynamicArray<IRenderSystem*>& EntityComponentSystem::GetRenderSystems() const {
	return renderSystems;
}
