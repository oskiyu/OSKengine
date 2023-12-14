#include "CollisionTests.h"

#include <format>

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/CollisionEvent.h>

void CollisionTesting::Start(OSK::ECS::GameObjectIndex firstObj, OSK::ECS::GameObjectIndex secondObj) {
	firstObject = firstObj;
	secondObject = secondObj;

	isExecuting = true;
}

void CollisionTesting::Update() {
	if (!isExecuting)
		return;

	const TDeltaTime timeSinceLastCase = OSK::Engine::GetCurrentTime() - lastTestChange;

	if (timeSinceLastCase > 3.0f) {
		currentTestIndex++;

		if (currentTestIndex >= testCases.GetSize()) {
			isExecuting = false;
			currentTestIndex = -1;

			return;
		}

		lastTestChange = OSK::Engine::GetCurrentTime();

		const bool isFirst = currentTestIndex % 16 < 8;

		OSK::Engine::GetLogger()->InfoLog(std::format("Test: {}", testCases[currentTestIndex].testName));
		// OSK::Engine::GetConsole()->WriteLine(std::format("Test: {}", testCases[currentTestIndex].testName));

		auto& transformA = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(firstObject);
		auto& transformB = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(secondObject);

		transformA.SetPosition(testCases[currentTestIndex].positionA);
		transformB.SetPosition(testCases[currentTestIndex].positionB);

		transformA.SetRotation({});
		transformB.SetRotation({});

		if (isFirst) {
			const OSK::Vector3f posDiff = testCases[currentTestIndex].positionA - testCases[currentTestIndex].positionB;
			const float angle = glm::radians(OSK::Vector3f(1.0f, 0.0f, 0.0f).GetAngle(posDiff));
			transformB.RotateWorldSpace(angle, OSK::Vector3f(0.0f, 1.0f, 0.0f));
		}
		else {
			const OSK::Vector3f posDiff = testCases[currentTestIndex].positionB - testCases[currentTestIndex].positionA;
			const float angle = glm::radians(OSK::Vector3f(1.0f, 0.0f, 0.0f).GetAngle(posDiff));
			transformA.RotateWorldSpace(angle, OSK::Vector3f(0.0f, 1.0f, 0.0f));
		}

		auto& physicsA = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::PhysicsComponent>(firstObject);
		auto& physicsB = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::PhysicsComponent>(secondObject);

		physicsA._SetVelocity(isFirst
			? OSK::Vector3f::Zero
			: testCases[currentTestIndex].positionB - testCases[currentTestIndex].positionA);
		physicsA._SetAcceleration(OSK::Vector3f::Zero);
		physicsA._SetAngularVelocity(OSK::Vector3f::Zero);

		physicsB._SetVelocity(!isFirst
			? OSK::Vector3f::Zero
			: testCases[currentTestIndex].positionA - testCases[currentTestIndex].positionB);
		physicsB._SetAcceleration(OSK::Vector3f::Zero);
		physicsB._SetAngularVelocity(OSK::Vector3f::Zero);
	}

	for (const auto& event : OSK::Engine::GetEcs()->GetEventQueue<OSK::ECS::CollisionEvent>()) {
		if ((event.firstEntity == firstObject || event.secondEntity == secondObject) && (event.firstEntity == secondObject || event.secondEntity == firstObject)) {
			OSK::Engine::GetLogger()->InfoLog(std::format("Collision: A: {}, B: {}, Position: {},{},{}",
				event.firstEntity, event.secondEntity,
				event.collisionInfo.GetSingleContactPoint().x,
				event.collisionInfo.GetSingleContactPoint().y,
				event.collisionInfo.GetSingleContactPoint().z));
			//	Engine::GetConsole()->WriteLine(std::format("Collision: A: {}, B: {}, Position: {},{},{}",
			//		event.firstEntity, event.secondEntity,
			//		event.collisionInfo.GetSingleContactPoint().x,
			//		event.collisionInfo.GetSingleContactPoint().y,
			//		event.collisionInfo.GetSingleContactPoint().Z));
		}
	}
}
