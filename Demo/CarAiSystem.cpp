#include "CarAiSystem.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/Math.h>

#include <OSKengine/Spline3D.h>

#include "CarComponent.h"
#include "CarAiComponent.h"
#include "CircuitComponent.h"

#include <OSKengine/CollisionSystem.h>

#include <random>

#include "RayCastEvent.h"
#include "CarControllerComponent.h"


void CarAiSystem::OnCreate() {
	OSK::ECS::Signature signature{};

	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarAiComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarControllerComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<OSK::ECS::PhysicsComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<OSK::ECS::Transform3D>());

	this->_SetSignature(signature);
}

void CarAiSystem::OnObjectAdded(OSK::ECS::GameObjectIndex car) {
	std::random_device dev;

	Rng rng{};
	rng.engine = std::mt19937(dev());
	rng.engine.seed(car);
	rng.distribution = std::uniform_int_distribution<UIndex32>(0, 10);

	rngs[car] = rng;
}

void CarAiSystem::OnTick(TDeltaTime deltaTime) {
	if (m_circuit == OSK::ECS::EMPTY_GAME_OBJECT) {
		return;
	}

	const auto& spline = OSK::Engine::GetEcs()->GetComponent<CircuitComponent>(m_circuit).trazadaIdeal->Get();

	for (const auto obj : GetObjects()) {
		auto& carTransform = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(obj);
		auto& carPhysics = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::PhysicsComponent>(obj);
		const auto& carComponent = OSK::Engine::GetEcs()->GetComponent<CarComponent>(obj);
		auto& carController = OSK::Engine::GetEcs()->GetComponent<CarControllerComponent>(obj);

		carController.Reset();

		const bool isOnFloor = carTransform.GetTopVector().Dot(OSK::Vector3f(0.0f, 1.0f, 0.0f)) > 0.96f;
		if (!isOnFloor)
			continue;

		const auto splinePointCount = spline.GetPointCount();

		CarAiComponent& carAi = OSK::Engine::GetEcs()->GetComponent<CarAiComponent>(obj);
		auto& curentPointId = carAi.curentPointId;
		const auto previousPointId = (curentPointId + splinePointCount - 1) % splinePointCount;
		const auto nextPointId = (curentPointId + 1) % splinePointCount;
		
		const auto& carPosition = carTransform.GetPosition();
		const auto& carForward = carTransform.GetForwardVector();
		const auto& carRight = carTransform.GetRightVector();

		const auto previousPoint = spline.GetPoint(previousPointId);
		const auto currentPoint = spline.GetPoint(curentPointId);
		const auto nextPoint = spline.GetPoint(curentPointId + 1);

		
		// Curve
		const OSK::Vector3f towardsPoint = currentPoint - previousPoint;
		/*const OSK::Vector3f carBetweenPoints = carPosition - previousPoint;
		const float carTowardsPointDot = carBetweenPoints.Dot(towardsPoint.GetNormalized());

		float intervalPercent = carTowardsPointDot / towardsPoint.GetLenght();
		const auto nPointId = previousPointId;
		OSK::Vector3f curvePoint = OSK::Vector3f(
			xIntervals[nPointId].Calculate(intervalPercent),
			yIntervals[nPointId].Calculate(intervalPercent),
			zIntervals[nPointId].Calculate(intervalPercent)
		);*/

		std::array<SplineInterval, 4> splineIntervalsX;
		std::array<SplineInterval, 4> splineIntervalsZ;

		splineIntervalsX[0] = CalculateIntervalWithDerivate1D(carForward.x, carPosition.x, currentPoint.x, nextPoint.x);
		splineIntervalsZ[0] = CalculateIntervalWithDerivate1D(carForward.z, carPosition.z, currentPoint.z, nextPoint.z);

		for (UIndex64 i = 1; i < splineIntervalsX.size(); i++) {
			const UIndex64 a = i + spline.GetPointCount() - 1;
			const UIndex64 b = i;
			const UIndex64 c = i + 1;
			const UIndex64 d = i + 2;

			const OSK::Vector3f A = spline.GetPoint(a);
			const OSK::Vector3f B = spline.GetPoint(b);
			const OSK::Vector3f C = spline.GetPoint(c);
			const OSK::Vector3f D = spline.GetPoint(d);

			splineIntervalsX[i] = CalculateInterval1D(A.x, B.x, C.x, D.x);
			splineIntervalsZ[i] = CalculateInterval1D(A.z, B.z, C.z, D.z);
		}


		const OSK::Vector3f projectedPoint = carPosition + carForward * 0.2f;

		OSK::Vector3f toProjectedPoint = projectedPoint - carPosition;
		OSK::Vector3f toNextPoint = currentPoint - carPosition;

		float intervalPercent = glm::abs(toProjectedPoint.Dot(toNextPoint.GetNormalized()));

		UIndex64 curvePointId = 0;
		for (UIndex64 i = 1; i < splineIntervalsX.size(); i++) {
			if (intervalPercent <= 1.0f) {
				break;
			}
			curvePointId = i;

			toProjectedPoint = projectedPoint - spline.GetPoint(curentPointId + 1);
			toNextPoint = spline.GetPoint(curentPointId + 2) - spline.GetPoint(curentPointId + 1);

			intervalPercent = toProjectedPoint.Dot(toNextPoint.GetNormalized());
		}

		carAi.nextPoint = OSK::Vector3f(
			splineIntervalsX[curvePointId].Calculate(intervalPercent),
			0.0f,
			splineIntervalsZ[curvePointId].Calculate(intervalPercent)
		);
		carAi.nextPoint = currentPoint;
		
		/*OSK::Engine::GetLogger()->InfoLog(std::format("\tIntervalPercent: {:.2}", intervalPercent));
		OSK::Engine::GetLogger()->InfoLog(std::format("\tCurrent point ID: {}", curentPointId));
		OSK::Engine::GetLogger()->InfoLog(std::format("\tCurve point ID: {}", nPointId));
		OSK::Engine::GetLogger()->InfoLog(std::format("\tCurvePoint: {:.4} {:.4}", curvePoint.x, curvePoint.z));
		OSK::Engine::GetLogger()->InfoLog(std::format("\tCurrentPoint: {:.4} {:.4}\n", currentPoint.x, currentPoint.z));*/
		

		// const auto currentDiff = currentPoint - carPosition;
		const auto currentDiff = carAi.nextPoint - carPosition;
		

		float currentAngle = currentDiff.GetAngle(carForward);

		//currentAngle = OSK::Vector3f(carForward.x, 0.0f, carForward.z).GetAngle(carAi.nextPoint);

		float currentRightDot = currentDiff.Dot(carRight);
		// currentRightDot = -OSK::Vector3f(carForward.x, 0.0f, carForward.z).Dot(carAi.nextPoint);

		if (currentRightDot > 0.0f) {
			currentAngle = -currentAngle;
		}

		carController.rightTurn = -glm::clamp(currentAngle / carComponent.maxTurning, -1.0f, 1.0f);


		bool isCarOnLeft = false;
		bool isCarOnRight = false;
		bool isCarOnFront = false;

		const float brakePerMeter = 0.35f * 1.45f;
		const float currentSpeed = carPhysics.GetVelocity().Dot(carForward);


		// Collision avoidance
#ifdef LOOP_AVOIDANCE
		float avoidanceStrength = 1.0f;
		for (const auto obj2 : GetObjects()) {
			if (obj2 == obj) {
				continue;
			}

			const auto& otherTransform = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(obj2);
			const auto& otherPhysics = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::PhysicsComponent>(obj2);

			// New:
			const auto tVec = (carPosition - otherTransform.GetPosition()) / (otherPhysics.GetVelocity() - carPhysics.GetVelocity());

			const float t_x = tVec.x;
			const float t_y = tVec.y;
			const float t_z = tVec.z;

			if (glm::abs(t_x - t_y) > 3.5f || glm::abs(t_y - t_z) > 3.5f) {
				continue;
			}

			const float t = (t_x + t_y + t_z) * 0.3333f;

			if (t < 0.0f) {
				continue;
			}

			const auto contactPosition = carPosition + carPhysics.GetVelocity() * t;

			const auto diff = contactPosition - carPosition;
			const auto carsDiff = otherTransform.GetPosition() - carPosition;

			const float longitudinalDiff = diff.Dot(carForward);
			const float horizontalDiff = carsDiff.Dot(carRight);

			if (t > 5.5f) {
				continue;
			}

			if (horizontalDiff < 0.0f &&
				(carPhysics.GetVelocity().GetDistanceTo(otherPhysics.GetVelocity()) > 10.0f
					|| carForward.GetAngle(otherTransform.GetForwardVector()) > 30.0f)) {
				isCarOnFront = true;
			}

			if (horizontalDiff > 0.0f) {
				isCarOnRight = true;
			}
			else {
				isCarOnLeft = true;
			}


			const auto diff = otherTransform.GetPosition() - carPosition;
			const auto velocityDiff = otherPhysics.GetVelocity() - carPhysics.GetVelocity();

			const float horizontalDiff = diff.Dot(carRight);
			const float longitudinalDiff = diff.Dot(carForward);

			if (glm::abs(longitudinalDiff) > 10.0f || glm::abs(horizontalDiff) > 2.1f) {
				continue;
			}


			if (longitudinalDiff > 0.0f && velocityDiff.Dot(carForward) > 5.0f) {
				isCarOnFront = true;
			}

			if (velocityDiff.Dot(carForward) < 5.0f && glm::abs(longitudinalDiff) > 3.0f) {
				continue;
			}

			if (horizontalDiff > 0.0f) {
				isCarOnRight = true;
			}
			else {
				isCarOnLeft = true;
			}

		}

#endif // LOOP_AVOIDANCE
		
		// Ray-cast avoidance.
		auto* collisionSystem = OSK::Engine::GetEcs()->GetSystem<OSK::ECS::CollisionSystem>();
		const float yOffset = 0.5f;

		OSK::COLLISION::Ray leftRay{};
		leftRay.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		leftRay.direction = (carForward * 0.9f - carRight * 0.1f);

		OSK::COLLISION::Ray leftRay2{};
		leftRay2.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		leftRay2.direction = (carForward * 0.8f - carRight * 0.2f);

		OSK::COLLISION::Ray leftRay3{};
		leftRay2.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		leftRay2.direction = (carForward * 0.6f - carRight * 0.4f);

		OSK::COLLISION::Ray rightRay{};
		rightRay.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		rightRay.direction = (carForward * 0.9f + carRight * 0.1f);

		OSK::COLLISION::Ray rightRay2{};
		rightRay2.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		rightRay2.direction = (carForward * 0.8f + carRight * 0.2f);

		OSK::COLLISION::Ray rightRay3{};
		rightRay2.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		rightRay2.direction = (carForward * 0.6f + carRight * 0.4f);

		OSK::COLLISION::Ray forwardRay{};
		forwardRay.origin = carPosition + OSK::Vector3f(0.0f, yOffset, 0.0f);
		forwardRay.direction = carForward;

		const auto leftRayCast = collisionSystem->CastRay(leftRay, obj);
		const auto rightRayCast = collisionSystem->CastRay(rightRay, obj);
		const auto leftRayCast2 = collisionSystem->CastRay(leftRay2, obj);
		const auto rightRayCast2 = collisionSystem->CastRay(rightRay2, obj);
		const auto leftRayCast3 = collisionSystem->CastRay(leftRay3, obj);
		const auto rightRayCast3 = collisionSystem->CastRay(rightRay3, obj);
		const auto forwardRayCast = collisionSystem->CastRay(forwardRay, obj);

		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, leftRayCast });
		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, rightRayCast });
		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, leftRayCast2 });
		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, rightRayCast2 });
		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, leftRayCast3 });
		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, rightRayCast3 });
		OSK::Engine::GetEcs()->PublishEvent<RayCastEvent>({ obj, forwardRayCast });

		if (leftRayCast.Result()) {
			const float distance = carPosition.GetDistanceTo(leftRayCast.GetIntersectionPoint());

			if (distance < 6.0f) {
				isCarOnLeft = true;
			}
		}

		if (leftRayCast2.Result()) {
			const float distance = carPosition.GetDistanceTo(leftRayCast2.GetIntersectionPoint());

			if (distance < 5.0f) {
				isCarOnLeft = true;
			}
		}

		if (leftRayCast3.Result()) {
			const float distance = carPosition.GetDistanceTo(leftRayCast3.GetIntersectionPoint());

			if (distance < 5.0f) {
				isCarOnLeft = true;
			}
		}

		if (rightRayCast.Result()) {
			const float distance = carPosition.GetDistanceTo(rightRayCast.GetIntersectionPoint());

			if (distance < 6.0f) {
				isCarOnRight = true;
			}
		}

		if (rightRayCast2.Result()) {
			const float distance = carPosition.GetDistanceTo(rightRayCast2.GetIntersectionPoint());

			if (distance < 5.0f) {
				isCarOnRight = true;
			}
		}

		if (rightRayCast3.Result()) {
			const float distance = carPosition.GetDistanceTo(rightRayCast3.GetIntersectionPoint());

			if (distance < 5.0f) {
				isCarOnRight = true;
			}
		}

		if (forwardRayCast.Result()) {
			const float distance = carPosition.GetDistanceTo(forwardRayCast.GetIntersectionPoint());

			if (distance < 5.0f) {
				// isCarOnFront = true;
			}
		}

		if (isCarOnLeft || isCarOnRight)
			carController.rightTurn = 0.0f;

		if (isCarOnLeft)
			carController.rightTurn += 1.0f;

		if (isCarOnRight)
			carController.rightTurn -= 1.0f;


		// if (glm::abs(currentAngle) > 0.01f) {
		//	carTransform.RotateWorldSpace(glm::radians(currentAngle) * 2.0f * deltaTime, { 0.0f, 1.0f, 0.0f });
		// }

		// Aceleración y frenada
		std::array<OSK::Vector3f, 8> points;
		std::array<float, 8> angles;
		std::array<float, 8> distances;

		for (UIndex64 i = 0; i < 8; i++) {
			const auto nPoint = spline.GetPoint(curentPointId + i) + OSK::Vector3f(carAi.diferencia);

			points[i] = nPoint;
			distances[i] = nPoint.GetDistanceTo(carPosition);
		}

		for (UIndex64 i = 0; i < 8 - 1; i++) {
			angles[i] = (points[i] - carPosition).GetNormalized().GetAngle((points[i + 1] - points[i]).GetNormalized());
		}

		carController.acceleration = 1.0f;

		for (UIndex64 i = 0; i < angles.size(); i++) {
			if (currentSpeed - brakePerMeter * distances[i] > carComponent.GetMaxSpeedForCorner(angles[i]) * 0.6f) {
				carController.acceleration = -1.0f;
				break;
			}
		}

		if (isCarOnFront) {
			carController.acceleration = -1.0f;
		}

		// carPhysics.ApplyForce(carTransform.GetForwardVector() * accel * 10.0f * 0.25f * 2.0f * 500.0f);

		const auto nextToNextNext = (points[1] - points[0]).GetNormalized();
		const auto carToNext = carPosition - points[0];

		const bool isNearPoint = carPosition.GetDistanceTo2(currentPoint) < 10.0f;
		const bool isPastPoint = carToNext.Dot(nextToNextNext) > 0.0f;

		if (isNearPoint || isPastPoint) {
			curentPointId = (curentPointId + 1) % (splinePointCount);
			carAi.diferencia = 1.0f - rngs.at(obj).Generate() * 0.1f * 0.5f;
		}
	}
}

void CarAiSystem::SetCircuit(OSK::ECS::GameObjectIndex circuit) {
	// s(t) = a*x*x*x + b*x*x + c*x + d
	// s'(x) = a*x*x + b*x + c

	m_circuit = circuit;
	const auto& spline = OSK::Engine::GetEcs()->GetComponent<CircuitComponent>(m_circuit).trazadaIdeal->Get();

	for (UIndex64 i = 0; i < spline.GetPointCount(); i++) {
		const UIndex64 a = (i + spline.GetPointCount() - 1) % spline.GetPointCount();
		const UIndex64 b = i;
		const UIndex64 c = (i + 1) % spline.GetPointCount();
		const UIndex64 d = (i + 2) % spline.GetPointCount();

		xIntervals.Insert(CalculateIntervalX(spline, a, b, c, d));
		yIntervals.Insert(CalculateIntervalY(spline, a, b, c, d));
		zIntervals.Insert(CalculateIntervalZ(spline, a, b, c, d));
	}
}

CarAiSystem::SplineInterval CarAiSystem::CalculateIntervalX(const OSK::Spline3D& spline, UIndex64 a, UIndex64 b, UIndex64 c, UIndex64 d) const {
	SplineInterval output{};

	const auto first = spline.GetPoint(a).x;
	const auto second = spline.GetPoint(b).x;
	const auto third = spline.GetPoint(c).x;
	const auto fourth = spline.GetPoint(d).x;

	return CalculateInterval1D(first, second, third, fourth);
}

CarAiSystem::SplineInterval CarAiSystem::CalculateIntervalY(const OSK::Spline3D& spline, UIndex64 a, UIndex64 b, UIndex64 c, UIndex64 d) const {
	SplineInterval output{};

	const auto first = spline.GetPoint(a).y;
	const auto second = spline.GetPoint(b).y;
	const auto third = spline.GetPoint(c).y;
	const auto fourth = spline.GetPoint(d).y;

	output.a = (-first + 3.0f * second - 3.0f * third + fourth) * 0.5f;
	output.b = first - 5.0f * second * 0.5f + 2 * third - fourth * 0.5f;
	output.c = (-first + third) * 0.5f;
	output.d = second;

	return output;
}

CarAiSystem::SplineInterval CarAiSystem::CalculateIntervalZ(const OSK::Spline3D& spline, UIndex64 a, UIndex64 b, UIndex64 c, UIndex64 d) const {
	SplineInterval output{};

	const auto first = spline.GetPoint(a).z;
	const auto second = spline.GetPoint(b).z;
	const auto third = spline.GetPoint(c).z;
	const auto fourth = spline.GetPoint(d).z;

	return CalculateInterval1D(first, second, third, fourth);
}

CarAiSystem::SplineInterval CarAiSystem::CalculateInterval1D(float a, float b, float c, float d) const {
	const auto firstDerivate = (c - a) * 0.5f;
	const auto secondDerivate = (d - b) * 0.5f;
	return CalculateIntervalInternal1D(firstDerivate, secondDerivate, b, c);
}

CarAiSystem::SplineInterval CarAiSystem::CalculateIntervalWithDerivate1D(float firstDerivate, float b, float c, float d) const {
	const float secondDerivate = (d - b) * 0.5f;
	return CalculateIntervalInternal1D(firstDerivate, secondDerivate, b, c);
}

CarAiSystem::SplineInterval CarAiSystem::CalculateIntervalInternal1D(float firstDerivate, float secondDerivate, float b, float c) const {
	SplineInterval output{};

	output.a = 2.0f * b - 2.0f * c + firstDerivate + secondDerivate;
	output.b = -3.0f * b + 3.0f * c - 2.0f * firstDerivate - secondDerivate;
	output.c = firstDerivate;
	output.d = b;

	return output;
}
