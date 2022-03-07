#include "SystemManager.h"

#include "ISystem.h"

using namespace OSK::ECS;

void SystemManager::OnTick(TDeltaTime deltaTime) {
	for (auto& i : sistemas)
		i.second->OnTick(deltaTime);
}

void SystemManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (auto& i : sistemas)
		i.second->RemoveObject(obj);
}

void SystemManager::GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature) {
	for (auto& i : sistemas) {
		if (signature.IsCompatible(i.second->GetSignature()))
			i.second->AddObject(obj);
		else
			i.second->RemoveObject(obj);
	}
}
