#pragma once

#include <OSKengine/IPureSystem.h>

class CameraAttachmentSystem : public OSK::ECS::IPureSystem {

public:

	OSK_SYSTEM("CameraAttachmentSystem");

	void OnCreate() override;
	void OnTick(TDeltaTime) override;

	void SetCar(OSK::ECS::GameObjectIndex carObject);
	void SetCamera(OSK::ECS::GameObjectIndex cameraObject);

private:

	OSK::ECS::GameObjectIndex m_cameraObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex m_carObject = OSK::ECS::EMPTY_GAME_OBJECT;

};
