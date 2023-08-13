#pragma once

#include "ComponentContainer.h"
#include "Component.h"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"

#include <string>

namespace OSK::ECS {

	/// @brief Se encarga de manejar la creación y eliminación de componentes.
	/// Guarda un ComponentContainer por cada tipo de componente registrado.
	class OSKAPI_CALL ComponentManager {

	public:

		ComponentManager() = default;
		OSK_DISABLE_COPY(ComponentManager);
		OSK_DEFAULT_MOVE_OPERATOR(ComponentManager);

		/// @brief Registra un componente, de tal manera que podrá ser usado
		/// en el ECS.
		/// @tparam TComponent Tipo del componente.
		template <typename TComponent> 
		void RegisterComponent() {
			const std::string key = static_cast<std::string>(TComponent::GetComponentTypeName());

			componentTypes[key] = nextComponentType;
			componentContainers[key] = new ComponentContainer<TComponent>();

			nextComponentType++;
		}

		/// @tparam TComponent Tipo de componente.
		/// @return Devuelve el identificador del tipo de componente.
		template <typename TComponent>
		ComponentType GetComponentType() const {
			return componentTypes.find(TComponent::GetComponentTypeName())->second;
		}

		/// @brief Añade el componente dado al GameObject dado.
		/// @tparam TComponent Tipo de componente.
		/// @param obj Identificador del objeto.
		/// @param component Componente a añadir.
		/// @return Referencia al componente.
		template <typename TComponent> 
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			return GetComponentContainer<TComponent>()->AddComponent(obj, component);
		}

		/// <summary>
		/// Añade el componente dado al GameObject dado.
		/// </summary>
		/// <returns>El componente recién añadido.</returns>
		template <typename TComponent> 
		TComponent& AddComponentMove(GameObjectIndex obj, TComponent&& component) {
			return GetComponentContainer<TComponent>()->AddComponentMove(obj, std::move(component));
		}

		/// <summary>
		/// Elimina el componente del objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		template <typename TComponent> 
		void RemoveComponent(GameObjectIndex obj) {
			GetComponentContainer<TComponent>()->RemoveComponent(obj);
		}

		/// <summary>
		/// Devuelve el componente del tipo dado, que es poseído por el objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		/// <param name="obj">Dueño del componente.</param>
		/// <returns>Referencia al componente.</returns>
		template <typename TComponent> 
		TComponent& GetComponent(GameObjectIndex obj) {
			return GetComponentContainer<TComponent>()->GetComponent(obj);
		}

		/// <summary>
		/// Se llama cuando un objeto es destruido.
		/// Se encarga de eliminar todos los componentes del objeto.
		/// </summary>
		void GameObjectDestroyed(GameObjectIndex obj);

		template <typename TComponent>
		bool ComponentHasBeenRegistered() const {
			return componentTypes.contains(TComponent::GetComponentTypeName());
		}

	private:

		/// <summary>
		/// Map typename del componente -> id del tipo de componente.
		/// </summary>
		std::unordered_map<std::string, ComponentType, StringHasher, std::equal_to<>> componentTypes;
		/// <summary>
		/// Map typename del componente -> contenedor del componente.
		/// </summary>
		std::unordered_map<std::string, UniquePtr<IComponentContainer>, StringHasher, std::equal_to<>> componentContainers;

		/// <summary>
		/// Identificador del próximo tipo de componente.
		/// </summary>
		ComponentType nextComponentType = 0;

		/// <summary>
		/// Devuelve el contenedor del tipo de componente.
		/// </summary>
		template <typename TComponent> ComponentContainer<TComponent>* GetComponentContainer() {
			return (ComponentContainer<TComponent>*)componentContainers.find(TComponent::GetComponentTypeName())->second.GetPointer();
		}
			
	};

}
