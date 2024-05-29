#include "ComponentManager.h"

using namespace OSK::ECS;

void ComponentManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (auto& [name, container] : componentContainers)
		container->GameObjectDestroyerd(obj);
}

std::string ComponentManager::GetComponentTypeName(ComponentType type) const {
	return m_typeToName.at(type);
}
