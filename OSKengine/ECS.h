#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "System.h"
#include "ComponentManager.h"
#include "GameObjectManager.h"

namespace OSK {

	class GameObject;

	class EntityComponentSystem {

	public:

		EntityComponentSystem() {
			systemManager = new ECS::SystemManager();
			componentManager = new ECS::ComponentManager();
			objectManager = new ECS::GameObjectManager();
		}
		~EntityComponentSystem() {
			delete systemManager;
			delete componentManager;
			delete objectManager;
		}

		void OnTick(deltaTime_t deltaTime) {
			systemManager->OnTick(deltaTime);
		}

		void OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
			systemManager->OnDraw(cmdBuffer, i);
		}

		ECS::GameObjectID CreateGameObject() {
			ECS::GameObjectID id = objectManager->CreateGameObject();

			return objectManager->CreateGameObject();
		}
		void DestroyGameObject(ECS::GameObjectID object) {
			systemManager->GameObjectDestroyed(object);
			componentManager->GameObjectDestroyed(object);
			objectManager->DestroyGameObject(object);
		}

		template <typename T> void RegisterComponent() {
			componentManager->RegisterComponent<T>();
		}
		template <typename C> void AddComponent(ECS::GameObjectID object, C component) {
			static_assert(std::is_base_of<Component, C>::value, "ERROR: AddComponent: not a Component!");

			componentManager->AddComponent(object, component);
			GetComponent<C>(object).OnCreate();

			Signature signature = objectManager->GetSignature(object);
			signature.set(componentManager->GetComponentType<C>(), true);
			objectManager->SetSignature(object, signature);

			systemManager->GameObjectSignatureChanged(object, signature);
		}

		template <typename T> void RemoveComponent(ECS::GameObjectID object) {
			GetComponent<T>(object).OnRemove();
			componentManager->RemoveComponent<T>(object);

			Signature signature = objectManager->GetSignature(object);
			signature.set(componentManager->GetComponentType<T>(), false);
			objectManager->SetSignature(object, signature);

			systemManager->GameObjectSignatureChanged(object, signature);
		}

		template <typename T> T& GetComponent(ECS::GameObjectID object) {
			return componentManager->GetComponent<T>(object);
		}

		template <typename T> ComponentType GetComponentType() {
			return componentManager->GetComponentType<T>();
		}

		template <typename T> T* RegisterSystem() {
			T* system = systemManager->CreateSystem<T>();
			system->ECSsystem = this;
			systemManager->SetSignature<T>(system->GetSystemSignature());
			system->OnCreate();

			return system;
		}
		template <typename T> void SetSystemSignature(Signature signature) {
			systemManager->SetSignature<T>(signature);
		}
		
		std::unordered_map<ECS::GameObjectID, GameObject*> GameObjects;

	private:

		ECS::SystemManager* systemManager = nullptr;
		ECS::ComponentManager* componentManager = nullptr;
		ECS::GameObjectManager* objectManager = nullptr;

	};

}