#include "ComponentManager.h"

using namespace OSK::ECS;

ComponentManager::~ComponentManager() {
	for (auto& i : componentContainers)
		delete i.second.GetPointer();
}

void ComponentManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (auto& i : componentContainers)
		i.second->GameObjectDestroyerd(obj);
}
