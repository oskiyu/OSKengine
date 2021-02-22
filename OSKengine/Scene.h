#pragma once

#include "ECS.h"
#include "GameObject.h"

#include <list>

namespace OSK {

	class OSKAPI_CALL Scene {

	public:

		Scene() {

		}

		~Scene() {

		}

		void AddGameObject(GameObject* object) {
			linkedObjects.push_back(object);
		}
		void RemoveGameObject(GameObject* object) {
			linkedObjects.remove(object);
		}

		void Update(deltaTime_t deltaTime) {
			for (auto i : linkedObjects) {
				i->OnTick(deltaTime);
			}
		}

	private:

		std::list<GameObject*> linkedObjects;
		EntityComponentSystem* ECS = nullptr;

	};

}
