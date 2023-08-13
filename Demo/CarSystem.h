#pragma once

#include <OSKengine/IConsumerSystem.h>

class CarSystem : public OSK::ECS::IConsumerSystem {

public:

	OSK_SYSTEM("CarSystem");

	void OnCreate() override;
	void OnTick(TDeltaTime) override;

};
