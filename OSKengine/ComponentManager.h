#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ComponentArray.h"
#include "Component.h"

#include <unordered_map>

namespace OSK::ECS {

	class ComponentManager {

	public:

		//Elimina los componentes.
		~ComponentManager() {
			for (auto& i : componentArray) {
				delete i.second;
			}
		}

		//Registra un componente para poder crear instancias de él.
		template <typename T> void RegisterComponent() {
			const char* tName = typeid(T).name();

			componentTypes.insert({ tName, nextComponentToBeRegistered });
			componentArray.insert({ tName, new ComponentArray<T>() });

			nextComponentToBeRegistered++;
		}

		//Obtiene el tipo de componente.
		template <typename T> ComponentType GetComponentType() {
			return componentTypes[typeid(T).name()];
		}

		//Añade un componente a un objeto.
		template <typename T> void AddComponent(GameObjectID object, T component) {
			GetComponentArray<T>()->InsertData(object, component);
		}

		//Elimina el componente de un objeto.
		template <typename T> void RemoveComponent(GameObjectID object) {
			GetComponentArray<T>()->RemoveData(object);
		}

		//Obtiene el componente de un objeto.
		template <typename T> T& GetComponent(GameObjectID object) {
			return GetComponentArray<T>()->GetData(object);
		}

		//Destruye todos los componentes de un objeto.
		void GameObjectDestroyed(GameObjectID object) {
			for (auto& i : componentArray) {
				i.second->GameObjectDestroyed(object);
			}
		}

	private:

		std::unordered_map<const char*, ComponentType> componentTypes;
		std::unordered_map<const char*, IComponentArray*> componentArray;

		ComponentType nextComponentToBeRegistered = 0;

		template <typename T> ComponentArray<T>* GetComponentArray() {
			return (ComponentArray<T>*)componentArray[typeid(T).name()];
		}

	};

}
