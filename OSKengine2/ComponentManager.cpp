#include "ComponentManager.h"

using namespace OSK::ECS;

void ComponentManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (const auto& [name, container] : componentContainers)
		container->GameObjectDestroyerd(obj);
}
