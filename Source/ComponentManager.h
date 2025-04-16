#pragma once

#include "ComponentContainer.h"
#include "Component.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"

#include "EcsExceptions.h"

#include <string>

namespace OSK::ECS {

	/// @brief Se encarga de manejar la creaci�n y eliminaci�n de componentes.
	/// Guarda un ComponentContainer por cada tipo de componente registrado.
	class OSKAPI_CALL ComponentManager {

	public:

		ComponentManager() = default;
		OSK_DISABLE_COPY(ComponentManager);
		OSK_DEFAULT_MOVE_OPERATOR(ComponentManager);

		/// @brief Registra un componente, de tal manera que podr� ser usado
		/// en el ECS.
		/// @tparam TComponent Tipo del componente.
		/// @pre @p TComponent no debe haber sido previamente registrado.
		/// @throws ComponentAlreadyRegisteredException si el componente ya hab�a sido registrado.
		template <typename TComponent> 
		void RegisterComponent() {
			OSK_ASSERT(!ComponentHasBeenRegistered<TComponent>(), ComponentAlreadyRegisteredException(TComponent::GetComponentTypeName()));

			const std::string key = static_cast<std::string>(TComponent::GetComponentTypeName());

			componentTypes[key] = nextComponentType;
			m_typeToName[nextComponentType] = key;
			componentContainers[key] = MakeUnique<ComponentContainer<TComponent>>();
			componentContainers[key]->SetComponentType(nextComponentType);

			nextComponentType++;
		}

		/// @tparam TComponent Tipo de componente.
		/// @return Devuelve el identificador del tipo de componente.
		/// @pre @p TComponent debe haber sido previamente registrado.
		/// @throws ComponentNotRegisteredException si el componente no hab�a sido registrado.
		template <typename TComponent>
		ComponentType GetComponentType() const {
			OSK_ASSERT(ComponentHasBeenRegistered<TComponent>(), ComponentNotRegisteredException(TComponent::GetComponentTypeName()))
			return componentTypes.find(TComponent::GetComponentTypeName())->second;
		}

		/// @brief A�ade el componente dado al GameObject dado.
		/// @tparam TComponent Tipo de componente.
		/// @param obj Identificador del objeto.
		/// @param component Componente a a�adir.
		/// @return Referencia al componente.
		template <typename TComponent> 
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			return GetComponentContainer<TComponent>()->AddComponent(obj, component);
		}

		/// <summary>
		/// A�ade el componente dado al GameObject dado.
		/// </summary>
		/// <returns>El componente reci�n a�adido.</returns>
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
		/// Devuelve el componente del tipo dado, que es pose�do por el objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		/// <param name="obj">Due�o del componente.</param>
		/// <returns>Referencia al componente.</returns>
		template <typename TComponent>
		TComponent& GetComponent(GameObjectIndex obj) {
			return GetComponentContainer<TComponent>()->GetComponent(obj);
		}

		/// <summary>
		/// Devuelve el componente del tipo dado, que es pose�do por el objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo del componente.</typeparam>
		/// <param name="obj">Due�o del componente.</param>
		/// <returns>Referencia al componente.</returns>
		template <typename TComponent>
		const TComponent& GetComponent(GameObjectIndex obj) const {
			return GetComponentContainer<TComponent>()->GetComponent(obj);
		}

		/// @param obj Objeto que contiene el componente
		/// buscado.
		/// @return Puntero al componente.
		/// Si el objeto no tiene el componente,
		/// devuelve nullptr.
		void* GetComponentAddress(GameObjectIndex obj, ComponentType type) {
			return componentContainers.at(m_typeToName.at(type))->GetComponentAddress(obj);
		}
		const void* GetComponentAddress(GameObjectIndex obj, ComponentType type) const {
			return componentContainers.at(m_typeToName.at(type))->GetComponentAddress(obj);
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

		/// @tparam TComponent Tipo de componente.
		/// @param obj Objeto a comprobar.
		/// @return True si el objeto tiene el componente indicado.
		/// 
		/// @pre El componente @p TComponent debe haber sido previamente registrado.
		/// @throws ComponentNotRegisteredException si @p TComponent no ha sido previamente registradao.
		template <typename TComponent>
		bool ObjectHasComponent(GameObjectIndex obj) const {
			OSK_ASSERT(ComponentHasBeenRegistered<TComponent>(), ComponentNotRegisteredException(TComponent::GetComponentTypeName()));
			return GetComponentContainer<TComponent>()->ObjectHasComponent(obj);
		}

		/// @return Todos los contenedores registrados.
		const std::unordered_map<std::string, UniquePtr<IComponentContainer>, StringHasher, std::equal_to<>>& GetAllContainers() const {
			return componentContainers;
		}

		/// @param componentTypeName Nombre del tipo de componente.
		/// @return Contenedor del tipo indicado.
		/// @pre El tipo de componente @p componentTypeName debe haber
		/// sido previamente registrado.
		/// @throws InvalidArgumentException Si se incumple la precondici�n.
		IComponentContainer* GetContainer(std::string_view componentTypeName) {
			auto iterator = componentContainers.find(componentTypeName);

			OSK_ASSERT(
				iterator != componentContainers.end(),
				InvalidArgumentException(std::format("El tipo de componente {} no est� registrado.", componentTypeName)));

			return iterator->second.GetPointer();
		}

		/// @param componentTypeName Nombre del tipo de componente.
		/// @return Contenedor del tipo indicado.
		/// @pre El tipo de componente @p componentTypeName debe haber
		/// sido previamente registrado.
		/// @throws InvalidArgumentException Si se incumple la precondici�n.
		const IComponentContainer* GetContainer(std::string_view componentTypeName) const {
			auto iterator = componentContainers.find(componentTypeName);

			OSK_ASSERT(
				iterator != componentContainers.end(),
				InvalidArgumentException(std::format("El tipo de componente {} no est� registrado.", componentTypeName)));

			return iterator->second.GetPointer();
		}

		/// @param type Tipo de componente.
		/// @return Nombre del tipo de componente.
		/// @pre @p type debe identificar un tipo de componente
		/// que haya sido previamente registrado.
		std::string GetComponentTypeName(ComponentType type) const;

	private:

		/// @brief Map typename del componente -> id del tipo de componente.
		std::unordered_map<std::string, ComponentType, StringHasher, std::equal_to<>> componentTypes;

		/// @brief ID del tipo de componente -> typename del componente.
		std::unordered_map<ComponentType, std::string> m_typeToName;

		/// @brief Map typename del componente -> contenedor del componente.
		std::unordered_map<std::string, UniquePtr<IComponentContainer>, StringHasher, std::equal_to<>> componentContainers;


		/// @brief Identificador del pr�ximo tipo de componente.
		ComponentType nextComponentType = 0;


		/// @tparam TComponent Tipo de componente.
		/// @return Contenedor del tipo de componente.
		/// @pre El tipo de componente @p TComponent debe haber
		/// sido previamente registrado.
		template <typename TComponent> 
		ComponentContainer<TComponent>* GetComponentContainer() {
			return (ComponentContainer<TComponent>*)componentContainers.find(TComponent::GetComponentTypeName())->second.GetPointer();
		}

		/// @tparam TComponent Tipo de componente.
		/// @return Contenedor del tipo de componente.
		/// @pre El tipo de componente @p TComponent debe haber
		/// sido previamente registrado.
		template <typename TComponent>
		const ComponentContainer<TComponent>* GetComponentContainer() const {
			return (const ComponentContainer<TComponent>*)componentContainers.find(TComponent::GetComponentTypeName())->second.GetPointer();
		}
		
	};

}
