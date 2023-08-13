#include "SystemManager.h"

#include "ISystem.h"

using namespace OSK::ECS;

void SystemManager::OnTick(TDeltaTime deltaTime) {
	for (const auto& [key, set] : executionOrder) {
		for (auto it = set.producers.begin(); it < set.producers.end(); ++it)
			if ((*it)->IsActive())
				(*it)->OnTick(deltaTime);

		for (IConsumerSystem* system : set.consumers)
			if (system->IsActive())
				system->OnTick(deltaTime);

		for (IPureSystem* system : set.systems)
			if (system->IsActive())
				system->OnTick(deltaTime);
	}	
}

void SystemManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (auto& [name, system] : producers)
		system->_RemoveObject(obj);

	for (auto& [name, system] : systems)
		system->_RemoveObject(obj);
}

void SystemManager::GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature) {
	for (auto& [name, system] : producers) {
		if (system->GetSignature().IsCompatible(signature))
			system->_AddObject(obj);
		else
			system->_RemoveObject(obj);
	}

	for (auto& [name, system] : systems) {
		if (system->GetSignature().IsCompatible(signature))
			system->_AddObject(obj);
		else
			system->_RemoveObject(obj);
	}
}
