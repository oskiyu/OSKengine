#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ECS.h"

namespace OSK {

	//Objeto del juego.
	class OSKAPI_CALL GameObject {

	public:

		virtual ~GameObject() = default;

		virtual void OnCreate() {}
		virtual void OnTick(deltaTime_t deltaTime) {}
		virtual void OnRemove() {}

		//ID del objeto
		ECS::GameObjectID ID;

		template <typename T> void AddComponent(T component) {
			ECSsystem->AddComponent<T>(ID, component);
		}
		template <typename T> void RemoveComponent() {
			ECSsystem->RemoveComponent<T>(ID);
		}

		template <typename T> T& GetComponent() {
			return ECSsystem->GetComponent<T>(ID);
		}

		void Create(EntityComponentSystem* ecs) {
			if (!hasBeenCreated) {
				ID = ecs->CreateGameObject();
				ECSsystem = ecs;
				OnCreate();
			}

			hasBeenCreated = true;
		}

		void Remove() {
			if (ECSsystem)
				ECSsystem->DestroyGameObject(ID);
		}

	private:

		EntityComponentSystem* ECSsystem = nullptr;

		bool hasBeenCreated = false;

	};

}
