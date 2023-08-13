#include "CarInputSystem.h"

#include <OSKengine/OSKengine.h>

#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/IKeyboardInput.h>
#include <OSKengine/IGamepadInput.h>
#include <OSKengine/IMouseInput.h>

#include "CarInputEvent.h"
#include "CarComponent.h"

void CarInputSystem::OnCreate() {
	OSK::ECS::Signature signature{};
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarComponent>());
	_SetSignature(signature);
}

void CarInputSystem::OnTick(TDeltaTime) {
	if (!this->GetObjects().ContainsElement(carObject))
		return;

	const OSK::IO::IKeyboardInput* keyboard = nullptr;
	const OSK::IO::IGamepadInput* gamepad = nullptr;
	const OSK::IO::IMouseInput* mouse = nullptr;

	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IKeyboardInput, (const void**)&keyboard);
	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IGamepadInput, (const void**)&gamepad);
	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IMouseInput, (const void**)&mouse);

	if (keyboard->IsKeyDown(OSK::IO::Key::W) || keyboard->IsKeyDown(OSK::IO::Key::UP)) {
		OSK::Engine::GetEcs()->PublishEvent<CarInputEvent>({ .type = CarInputEvent::Type::ACCELERATE, .carObject = carObject });
	}
	if (keyboard->IsKeyDown(OSK::IO::Key::S) || keyboard->IsKeyDown(OSK::IO::Key::DOWN)) {
		OSK::Engine::GetEcs()->PublishEvent<CarInputEvent>({ .type = CarInputEvent::Type::DECELERATE, .carObject = carObject });
	}
	if (keyboard->IsKeyDown(OSK::IO::Key::A) || keyboard->IsKeyDown(OSK::IO::Key::LEFT)) {
		OSK::Engine::GetEcs()->PublishEvent<CarInputEvent>({ .type = CarInputEvent::Type::TURN_LEFT,  .carObject = carObject });
	}
	if (keyboard->IsKeyDown(OSK::IO::Key::D) || keyboard->IsKeyDown(OSK::IO::Key::RIGHT)) {
		OSK::Engine::GetEcs()->PublishEvent<CarInputEvent>({ .type = CarInputEvent::Type::TURN_RIGHT, .carObject = carObject });
	}
}

void CarInputSystem::SetCar(OSK::ECS::GameObjectIndex carObject) {
	this->carObject = carObject;
}
