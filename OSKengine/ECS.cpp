#include "ECS.h"

using namespace OSK;

EntityComponentSystem::EntityComponentSystem() {
	systemManager = new ECS::SystemManager();
	componentManager = new ECS::ComponentManager();
	objectManager = new ECS::GameObjectManager();
}

void EntityComponentSystem::OnTick(deltaTime_t deltaTime) {
	systemManager->OnTick(deltaTime);
}

void EntityComponentSystem::OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
	systemManager->OnDraw(cmdBuffer, i);
}

ECS::GameObjectID EntityComponentSystem::CreateGameObject() {
	ECS::GameObjectID id = objectManager->CreateGameObject();

	return objectManager->CreateGameObject();
}

void EntityComponentSystem::DestroyGameObject(ECS::GameObjectID object) {
	systemManager->GameObjectDestroyed(object);
	componentManager->GameObjectDestroyed(object);
	objectManager->DestroyGameObject(object);
}