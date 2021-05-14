#include "SystemManager.h"

using namespace OSK;
using namespace OSK::ECS;

void SystemManager::OnTick(deltaTime_t deltaTime) {
	for (auto& i : systems)
		i.second->OnTick(deltaTime);
}

void SystemManager::OnDraw(VkCommandBuffer cmdBuffer, uint32_t ix) {
	for (auto& i : systems)
		i.second->OnDraw(cmdBuffer, ix);
}

void SystemManager::GameObjectDestroyed(GameObjectID obj) {
	for (auto& i : systems) {
		i.second->objects.erase(obj);
	}
}

void SystemManager::GameObjectSignatureChanged(GameObjectID object, Signature signature) {
	for (auto& i : systems) {
		const char* type = i.first;
		System* system = i.second;
		Signature systemSignature = signatures[type];

		// Entity signature matches system signature - insert into set
		if ((signature & systemSignature) == systemSignature)
			system->objects.insert(object);
		// Entity signature does not match system signature - erase from set
		else
			system->objects.erase(object);
	}
}