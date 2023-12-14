#pragma once

#include <OSKengine/IPureSystem.h>

class HubCameraSystem : public OSK::ECS::IPureSystem {

public:

	OSK_SYSTEM("HubCameraSystem");

	void OnCreate() override;
	void OnTick(TDeltaTime deltaTime) override;

	void OnObjectAdded(OSK::ECS::GameObjectIndex obj) override;

private:

};
