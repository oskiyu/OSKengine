#include "ComponentManager.h"

using namespace OSK::ECS;

ComponentManager::~ComponentManager() {
	for (auto& i : componentArray)
		delete i.second;
}

void ComponentManager::GameObjectDestroyed(GameObjectID object) {
	for (auto& i : componentArray)
		i.second->GameObjectDestroyed(object);
}