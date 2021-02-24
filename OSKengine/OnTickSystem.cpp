#include "OnTickSystem.h"

#include "OnTickComponent.h"

using namespace OSK;

Signature OnTickSystem::GetSystemSignature() {
	Signature signature;

	signature.set(ECSsystem->GetComponentType<OSK::OnTickComponent>());

	return signature;
}

void OnTickSystem::OnTick(deltaTime_t deltaTime) {
	for (auto obj : Objects) {
		auto& onTick = ECSsystem->GetComponent<OnTickComponent>(obj);

		onTick.OnTick(deltaTime);
	}
}