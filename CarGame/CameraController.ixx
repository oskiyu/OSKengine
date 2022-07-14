module;

#include <OSKengine/GameObject.h>
#include <OSKengine/KeyboardState.h>
#include <OSKengine/CameraComponent3D.h>
#include <OSKengine/Transform3D.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>
#include <OSKengine/Component.h>
#include <OSKengine/ISystem.h>
#include <OSKengine/IInputListener.h>

export module CameraController;

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;

export struct CameraControlMessage {
	float forwardMovement = 0.0f;
	float rightMovement = 0.0f;
};

export struct CameraControlComponent {

	OSK_COMPONENT("CameraControlComponent");

	float speed = 3.0f;

	DynamicArray<CameraControlMessage> messageQueue;

};

export class CameraControlSystem : public ISystem {

public:

	OSK_SYSTEM("CameraControlSystem");

	CameraControlSystem() {
		Signature signature{};

		signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<CameraControlComponent>());
		signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform3D>());

		SetSignature(signature);
	}

	void OnTick(float deltaTime) override {
		for (const GameObjectIndex obj : GetObjects()) {
			auto& controlComponent = Engine::GetEntityComponentSystem()->GetComponent<CameraControlComponent>(obj);
			auto& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj);

			for (const CameraControlMessage& msg : controlComponent.messageQueue) {
				if (msg.forwardMovement != 0.0f)
					transform.AddPosition(transform.GetForwardVector() * deltaTime * controlComponent.speed * msg.forwardMovement);
				if (msg.rightMovement != 0.0f)
					transform.AddPosition(transform.GetRightVector() * deltaTime * controlComponent.speed * msg.rightMovement);
			}

			controlComponent.messageQueue.Empty();
		}
	}

};

export class CameraInputListener : public IInputListener {

public:

	void OnKeyBeingPressed(Key key) override {
		if (controlledObject == EMPTY_GAME_OBJECT)
			return;

		auto& control = Engine::GetEntityComponentSystem()->GetComponent<CameraControlComponent>(controlledObject);

		switch (key) {

		case Key::W:
			control.messageQueue.Insert({ 1.0f, 0.0f });
			break;

		case Key::S:
			control.messageQueue.Insert({ -1.0f, 0.0f });
			break;

		case Key::A:
			control.messageQueue.Insert({ 0.0f, -1.0f });
			break;

		case Key::D:
			control.messageQueue.Insert({ 0.0f, 1.0f });
			break;

		default:
			break;

		}

	}

	void OnMouseMoved(const Vector2i& diff) override {
		if (false)
			return;

		Engine::GetEntityComponentSystem()->GetComponent<CameraComponent3D>(controlledObject).Rotate(
			(float)diff.X,
			(float)diff.Y
		);
	}

	inline void SetControlledCamera(GameObjectIndex cameraObject) {
		controlledObject = cameraObject;
	}

private:

	GameObjectIndex controlledObject = EMPTY_GAME_OBJECT;

};
