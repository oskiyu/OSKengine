#pragma once

#include <OSKengine/IPureSystem.h>

class CarSystem : public OSK::ECS::IPureSystem {

public:

	OSK_SYSTEM("CarSystem");

	void OnCreate() override;
	void OnTick(TDeltaTime) override;

};
