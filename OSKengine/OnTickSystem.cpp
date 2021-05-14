#include "OnTickSystem.h"

#include "OnTickComponent.h"

using namespace OSK;

Signature OnTickSystem::GetSystemSignature() {
	Signature signature;

	signature.set(entityComponentSystem->GetComponentType<OSK::OnTickComponent>());

	return signature;
}

void OnTickSystem::OnTick(deltaTime_t deltaTime) {
	for (auto obj : objects) {
		auto& onTick = entityComponentSystem->GetComponent<OnTickComponent>(obj);

		onTick.OnTick(deltaTime);
	}
}