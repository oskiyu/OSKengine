#pragma once

#include <OSKengine/IProducerSystem.h>

class CarInputSystem : public OSK::ECS::IProducerSystem {

public:

	OSK_SYSTEM("CarInputSystem");

	void OnCreate() override;
	void OnTick(TDeltaTime deltaTime) override;

	void SetCar(OSK::ECS::GameObjectIndex carObject);

private:

	OSK::ECS::GameObjectIndex carObject = OSK::ECS::EMPTY_GAME_OBJECT;

};
