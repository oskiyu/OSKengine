module;

#include <OSKengine/ISystem.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>
#include <OSKengine/Transform3D.h>
#include <OSKengine/Math.h>

export module CarSystem;

export import CarComponent;

using namespace OSK;
using namespace OSK::ECS;

inline float LinearInterpolation_Fast(float a, float b, float factor) {
	return (a * (1.0f - factor)) + (b * factor);
}

inline float LinearInterpolation(float a, float b, float factor) {
	if (factor >= 1.0f)
		return b;

	if (factor <= 0.0f)
		return a;

	return LinearInterpolation_Fast(a, b, factor);
}

#define OSK_MS_TO_KMH(x) x * 3600 / 1000
#define OSK_KMH_TO_MS(x) x * 1000 / 3600

export class CarSystem : public ISystem {

public:

	OSK_SYSTEM("CarSystem");

	CarSystem() {
		Signature signature{};

		signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<CarComponent>());
		signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform3D>());

		SetSignature(signature);
	}

	void OnTick(float deltaTime) override {

		for (const auto i : GetObjects()) {
			auto& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(i);
			auto& carComponent = Engine::GetEntityComponentSystem()->GetComponent<CarComponent>(i);

			ProcessMessages(carComponent);

			const Vector3f aeroNrake = -carComponent.speed * 0.05f;
			const Vector3f motorBrake = -carComponent.speed * 0.15f;

			// Si hay giro
			if (glm::abs(carComponent.anglePercent) > 0.1f) {

				// Clamp
				if (carComponent.accelPercent > 1.0f)
					carComponent.accelPercent = 1.0f;
				else if (carComponent.accelPercent < -1.0f)
					carComponent.accelPercent = -1.0f;
				
				// Radio = distancia entre ejes / sen(ángulo)
				const float wheelBase = 1.6f;
				const float turnRadius = wheelBase / glm::sin(glm::radians(carComponent.anglePercent));

				// Velocidad angular = velocidad lineal / radio
				const float angularVelocity = carComponent.speed / turnRadius;
				transform.RotateWorldSpace(glm::degrees(angularVelocity * deltaTime), { 0, -1, 0 });
			}

			carComponent.speed += carComponent.accelPercent * deltaTime;
			carComponent.speed -= aeroNrake.GetLenght() * deltaTime;

			if (carComponent.accelPercent < 0.1f)
				carComponent.speed -= motorBrake.GetLenght() * deltaTime;

			transform.AddPosition(transform.GetForwardVector() * carComponent.speed * deltaTime * 5.0f);

			carComponent.accelPercent = 0.0f;
			carComponent.anglePercent = 0.0f;
		}

	}

private:

	inline void ProcessMessages(CarComponent& component) {
		for (const CarControlMessage& msg : component.controller) {

			switch (msg.type) {

			case CarControlMessage::Type::ACCEL:
				component.accelPercent = msg.amount;
				break;

			case CarControlMessage::Type::STOP:
				component.accelPercent = 0.0f;
				component.anglePercent = 0.0f;
				break;

			case CarControlMessage::Type::TURN:
				component.anglePercent = msg.amount;
				break;

			}

		}

		component.controller.Empty();
	}

};
