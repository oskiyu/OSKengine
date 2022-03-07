#pragma once

#include "ComponentContainer.h"
#include "Component.h"
#include "HashMap.hpp"
#include "OwnedPtr.h"

#include <string>

namespace OSK::ECS {

	/// <summary>
	/// Se encarga de manejar la creación y eliminación de componentes.
	/// Guarda un ComponentContainer por cada tipo de componente registrado.
	/// </summary>
	class OSKAPI_CALL ComponentManager {

	public:

		~ComponentManager();

		/// <summary>
		/// Registra un componente, de tal manera que podrá ser usado
		/// en el ECS.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		template <typename TComponent> void RegisterComponent() {
			std::string key = TComponent::GetComponentTypeName();

			componentTypes.Insert(key, nextComponentType);
			componentContainers.Insert(key, new ComponentContainer<TComponent>());

			nextComponentType++;
		}

		/// <summary>
		/// Devuelve el identificador del tipo de componente.
		/// </summary>
		template <typename TComponent> ComponentType GetComponentType() const {
			return componentTypes.Get(TComponent::GetComponentTypeName());
		}

		/// <summary>
		/// Añade el componente dado al GameObject dado.
		/// </summary>
		/// <returns>El componente recién añadido.</returns>
		template <typename TComponent> TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			return GetComponentContainer<TComponent>()->AddComponent(obj, component);
		}

		/// <summary>
		/// Elimina el componente del objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		template <typename TComponent> void RemoveComponent(GameObjectIndex obj) {
			GetComponentContainer<TComponent>()->RemoveComponent(obj);
		}

		/// <summary>
		/// Devuelve el componente del tipo dado, que es poseído por el objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		/// <param name="obj">Dueño del componente.</param>
		/// <returns>Referencia al componente.</returns>
		template <typename TComponent> TComponent& GetComponent(GameObjectIndex obj) const {
			return GetComponentContainer<TComponent>()->GetComponent(obj);
		}

		/// <summary>
		/// Se llama cuando un objeto es destruido.
		/// Se encarga de eliminar todos los componentes del objeto.
		/// </summary>
		void GameObjectDestroyed(GameObjectIndex obj);

	private:

		/// <summary>
		/// Map typename del componente -> id del tipo de componente.
		/// </summary>
		HashMap<std::string, ComponentType> componentTypes;
		/// <summary>
		/// Map typename del componente -> contenedor del componente.
		/// </summary>
		HashMap<std::string, OwnedPtr<IComponentContainer>> componentContainers;

		/// <summary>
		/// Identificador del próximo tipo de componente.
		/// </summary>
		ComponentType nextComponentType = 0;

		/// <summary>
		/// Devuelve el contenedor del tipo de componente.
		/// </summary>
		template <typename TComponent> ComponentContainer<TComponent>* GetComponentContainer() const {
			return (ComponentContainer<TComponent>*)componentContainers.Get(TComponent::GetComponentTypeName()).GetPointer();
		}
			
	};

}
