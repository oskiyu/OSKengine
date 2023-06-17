#include "pch.h"
#include "CppUnitTest.h"

#include "../OSKengine2/Component.h"
#include "../OSKengine2/EntityComponentSystem.h"
#include "../OSKengine2/IPureSystem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace OSK;
using namespace OSK::ECS;


struct TestComponent {
	OSK_COMPONENT("TestComponent")
		int value = 1;
};

struct TestSystem : public IPureSystem {
	OSK_SYSTEM("TestSystem");
	void SetSingature(ComponentType type) {
		auto signature = Signature();
		signature.SetTrue(type);
		_SetSignature(signature);
	}
	void OnTick(TDeltaTime) override {
		for (const auto& id : GetObjects())
			value++;
	}
	int value = 1;
};

struct TestEvent {
	OSK_EVENT("TestEvent");
	int value = 0;

	bool operator==(const TestEvent& other) const = default;
};

TEST_CLASS(EcsTests) {

public:

	static inline std::unique_ptr<EntityComponentSystem> ecs = nullptr;
	static inline GameObjectIndex obj = EMPTY_GAME_OBJECT;

	TEST_METHOD_INITIALIZE(Init) {
		ecs = std::make_unique<EntityComponentSystem>(nullptr);
		obj = ecs->SpawnObject();
		Assert::AreNotEqual(obj, EMPTY_GAME_OBJECT, L"No se ha conseguido crear el objeto.");
	}

#pragma region Components

	TEST_METHOD(TryAddToNonExistingEntity) {
		Assert::ExpectException<InvalidObjectException>([]() { ecs->AddComponent(EMPTY_GAME_OBJECT, TestComponent{}); });
	}

	TEST_METHOD(RegisterComponent) {
		ecs->RegisterComponent<TestComponent>();
	}

	TEST_METHOD(AddComponent) {
		ecs->RegisterComponent<TestComponent>();
		ecs->AddComponent(obj, TestComponent{});

		Assert::IsTrue(ecs->ObjectHasComponent<TestComponent>(obj));
	}

	TEST_METHOD(AddSecondComponentError) {
		ecs->RegisterComponent<TestComponent>();
		ecs->AddComponent(obj, TestComponent{});

		Assert::ExpectException<ObjectAlreadyHasComponentException>([]() { ecs->AddComponent(obj, TestComponent{}); });
	}

	TEST_METHOD(RemoveComponent) {
		ecs->RegisterComponent<TestComponent>();
		ecs->AddComponent(obj, TestComponent{});

		ecs->RemoveComponent<TestComponent>(obj);

		Assert::IsFalse(ecs->ObjectHasComponent<TestComponent>(obj));
	}

	TEST_METHOD(RemoveSecondComponentError) {
		ecs->RegisterComponent<TestComponent>();
		ecs->AddComponent(obj, TestComponent{});
		ecs->RemoveComponent<TestComponent>(obj);

		Assert::ExpectException<ComponentNotFoundException>([]() { ecs->RemoveComponent<TestComponent>(obj); });
	}

#pragma endregion

#pragma region System

	TEST_METHOD(RegisterSystem) {
		ecs->RegisterSystem<TestSystem>();
		Assert::IsTrue(ecs->HasSystem<TestSystem>());
	}

	TEST_METHOD(RegisterSecondSystem) {
		ecs->RegisterSystem<TestSystem>();
		Assert::ExpectException<SystemAlreadyRegisteredException>([]() { ecs->RegisterSystem<TestSystem>(); });
	}

	TEST_METHOD(RemoveSystem) {
		ecs->RegisterSystem<TestSystem>();
		ecs->RemoveSystem<TestSystem>();
		Assert::IsFalse(ecs->HasSystem<TestSystem>());
	}

	TEST_METHOD(RemoveSecondSystem) {
		ecs->RegisterSystem<TestSystem>();
		ecs->RemoveSystem<TestSystem>();
		Assert::ExpectException<SystemNotFoundException>([]() { ecs->RemoveSystem<TestSystem>(); });
	}

	TEST_METHOD(GetNonExistingSystem) {
		Assert::ExpectException<SystemNotFoundException>([]() { const auto _ =ecs->GetSystem<TestSystem>(); });
	}

	TEST_METHOD(SystemExecutionWithComponent) {
		auto system = ecs->RegisterSystem<TestSystem>();
		ecs->RegisterComponent<TestComponent>();
		ecs->AddComponent(obj, TestComponent());

		system->SetSingature(ecs->GetComponentType<TestComponent>());

		ecs->OnTick(1.0f);

		Assert::AreEqual(system->value, 2);

		ecs->RemoveSystem<TestSystem>();
	}

	TEST_METHOD(SystemExecutionWithoutComponent) {
		auto system = ecs->RegisterSystem<TestSystem>();
		ecs->RegisterComponent<TestComponent>();

		ecs->OnTick(1.0f);

		Assert::AreEqual(system->value, 1);
	}

#pragma endregion

#pragma region Events

	TEST_METHOD(RegsiterEvent) {
		ecs->RegisterEventType<TestEvent>();
		Assert::IsTrue(ecs->EventHasBeenRegistered<TestEvent>());
	}

	TEST_METHOD(PushEvent) {
		ecs->RegisterEventType<TestEvent>();
		Assert::IsTrue(ecs->EventHasBeenRegistered<TestEvent>());

		auto event = TestEvent();
		event.value = 2;
		ecs->PublishEvent(event);

		const auto& event2 = ecs->GetEventQueue<TestEvent>().At(0);

		Assert::AreEqual(event.value, event2.value);
		Assert::AreEqual(event.value, 2);
	}

	TEST_METHOD(PushNonRegisteredEvent) {
		Assert::IsFalse(ecs->EventHasBeenRegistered<TestEvent>());
		Assert::ExpectException<EventNotRegisteredException>([]() { ecs->PublishEvent(TestEvent()); });
	}

	TEST_METHOD(GetNonRegisteredEventQueue) {
		Assert::IsFalse(ecs->EventHasBeenRegistered<TestEvent>());
		Assert::ExpectException<EventNotRegisteredException>([]() { ecs->GetEventQueue<TestEvent>(); });
	}
	
#pragma endregion

};
