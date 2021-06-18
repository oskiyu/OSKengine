#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ComponentArray.h"
#include "Component.h"

#include <unordered_map>

namespace OSK::ECS {

	/// <summary>
	/// Clase que maneja los componentes.
	/// Se usa para registrar componentes, y para añadir componentes a entidades.
	/// </summary>
	class ComponentManager {

	public:

		/// <summary>
		/// Elimina los componentes.
		/// <summary/>
		OSKAPI_CALL ~ComponentManager();

		/// <summary>
		/// Registra un componente para poder crear instancias de él.
		/// </summary>
		/// <typeparam name="T">Componente.</typeparam>
		template <typename T> void RegisterComponent() {
			std::string key = T::GetComponentName();

			componentTypes.insert({ key, nextComponentToBeRegistered });
			componentArray.insert({ key, new ComponentArray<T>() });

			nextComponentToBeRegistered++;
		}

		/// <summary>
		/// Obtiene el tipo de componente.
		/// </summary>
		/// <typeparam name="T">Componente</typeparam>
		/// <returns>Su tipo.</returns>
		template <typename T> ComponentType GetComponentType() {
			return componentTypes.at(T::GetComponentName());
		}

		/// <summary>
		/// Añade un componente a un objeto.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="object">ID del objeto.</param>
		/// <param name="component">Componente.</param>
		template <typename T> T& AddComponent(GameObjectID object, T component) {
			return GetComponentArray<T>()->InsertData(object, component);
		}

		/// <summary>
		/// Elimina el componente de un objeto.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="object">ID del objeto.</param>
		template <typename T> void RemoveComponent(GameObjectID object) {
			GetComponentArray<T>()->RemoveData(object);
		}

		/// <summary>
		/// Obtiene el componente de un objeto.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="object">ID del objeto.</param>
		/// <returns>Componente.</returns>
		template <typename T> T& GetComponent(GameObjectID object) {
			return GetComponentArray<T>()->GetData(object);
		}

		/// <summary>
		/// Destruye todos los componentes de un objeto.
		/// </summary>
		/// <param name="object">ID del objeto.</param>
		OSKAPI_CALL void GameObjectDestroyed(GameObjectID object);

	private:

		/// <summary>
		/// Map nombre de componente -> tipo de componente.
		/// </summary>
		std::unordered_map<std::string, ComponentType> componentTypes;

		/// <summary>
		/// Map nombre de componente -> array de componente.
		/// </summary>
		std::unordered_map<std::string, IComponentArray*> componentArray;

		/// <summary>
		/// ID del próximo componente a registrar.
		/// </summary>
		ComponentType nextComponentToBeRegistered = 0;

		/// <summary>
		/// Obtiene el array de componentes de un tipo.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <returns>Array.</returns>
		template <typename T> ComponentArray<T>* GetComponentArray() {
			std::string key = T::GetComponentName();

			return (ComponentArray<T>*)componentArray.at(key);
		}

	};

}
