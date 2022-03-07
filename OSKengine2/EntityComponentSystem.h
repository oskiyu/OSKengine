#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "ComponentManager.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "ISystem.h"

namespace OSK::ECS {

	class OSKAPI_CALL EntityComponentSystem {

	public:

		EntityComponentSystem();

		void OnTick(TDeltaTime deltaTime);

		template <typename TComponent> void RegisterComponent() {
			componentManager->RegisterComponent<TComponent>();
		}

		template <typename TComponent> TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			TComponent& oComponent = componentManager->AddComponent(obj, component);

			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		template <typename TComponent> bool ObjectHasComponent(GameObjectIndex obj) const {
			return gameObjectManager->GetSignature(obj).Get(
				componentManager->GetComponentType<TComponent>()
			);
		}

		template <typename TComponent> void RemoveComponent(GameObjectIndex obj) {
			componentManager->RemoveComponent<TComponent>(obj);

			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetFalse(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);
		}

		template <typename TComponent> TComponent& GetComponent(GameObjectIndex obj) const {
			return componentManager->GetComponent<TComponent>(obj);
		}

		template <typename TComponent> ComponentType GetComponentType() const {
			return componentManager->GetComponentType<TComponent>();
		}

		template <typename TSystem> TSystem* RegisterSystem() {
			TSystem* output = systemManager->RegisterSystem<TSystem>();

			((ISystem*)output)->OnCreate();

			return output;
		}

		GameObjectIndex SpawnObject();
		void DestroyObject(GameObjectIndex* obj);

	private:

		UniquePtr<SystemManager> systemManager;
		UniquePtr<ComponentManager> componentManager;
		UniquePtr<GameObjectManager> gameObjectManager;

	};

}
