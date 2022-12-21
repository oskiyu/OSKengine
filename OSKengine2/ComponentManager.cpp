#include "ComponentManager.h"

using namespace OSK::ECS;

ComponentManager::~ComponentManager() {

}

void ComponentManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (auto& i : componentContainers)
		i.second->GameObjectDestroyerd(obj);
}
