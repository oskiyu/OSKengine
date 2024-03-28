#pragma once

#include "IComponentContainer.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "Component.h"
#include "Memory.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

namespace OSK::ECS {
		
	/// @brief Contenedor para el dynamic array de componentes del tipo dado.
	/// Se encarga de manejar la introducción y eliminación de componentes.
	/// Los componentes se almacenan en memoria contínua.
	/// @tparam TComponent Tipo del componente.
	/// 
	/// @pre TComponent debe validar IsEcsComponent.
	template <typename TComponent> requires IsEcsComponent<TComponent>
	class ComponentContainer : public IComponentContainer {

	public:

		~ComponentContainer() override = default;

		/// @brief Añade un componente asignado al objeto dado.
		/// @param obj Objeto dueño del componente.
		/// @param component Componente a añadir.
		/// @return Referencia no estable al componente añadido.
		/// 
		/// @pre El objeto @p obj no contiene previamente un componente del tipo.
		/// @throws InvalidArgumentException si se incumple la precondición.
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			OSK_ASSERT(
				!ObjectHasComponent(obj),
				InvalidArgumentException(std::format("El objeto {} ya contiene el componente.", obj)))

			ComponentIndex componentId = m_components.GetSize();

			m_objectToComponent[obj] = componentId;
			m_componentToObject[componentId] = obj;
			m_components.Insert(component);

			return GetComponent(obj);
		}

		/// @brief Añade un componente asignado al objeto dado.
		/// @param obj Objeto dueño del componente.
		/// @param component Componente a añadir.
		/// @return Referencia no estable al componente añadido.
		/// 
		/// @pre El objeto @p obj no contiene previamente un componente del tipo.
		/// @throws InvalidArgumentException si se incumple la precondición.
		TComponent& AddComponentMove(GameObjectIndex obj, TComponent&& component) {
			OSK_ASSERT(
				!ObjectHasComponent(obj),
				InvalidArgumentException(std::format("El objeto {} ya contiene el componente.", obj)))

			const ComponentIndex componentId = m_components.GetSize();

			m_objectToComponent[obj] = componentId;
			m_componentToObject[componentId] = obj;
			m_components.Insert(std::move(component));

			return GetComponent(obj);
		}
				
		/// @brief Elimina el componente cuyo dueño es el dado.
		/// @param obj Dueño del componente.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		/// @throws InvalidArgumentException Si el objeto no contiene el componente dado.
		void RemoveComponent(GameObjectIndex obj) {
			OSK_ASSERT(
				m_objectToComponent.contains(obj), 
				InvalidArgumentException(std::format("El objeto {} no contiene el componente.", obj)))

			ComponentIndex compIndex = m_objectToComponent.at(obj);
			const UIndex64 indexOfLast = m_components.GetSize() - 1;

			// Hacemos que el componente a eliminar esté en la última posición.
			// El último componente se coloca en el hueco del componente eliminado,
			// para evitar dejar huecos en el array.
			MEMORY::MemorySwap(&m_components[compIndex], &m_components[indexOfLast], sizeof(TComponent));

			// Obtenemos el objeto dueño del componente que acabamos de colocar
			// donde el componente eliminado.
			// Actualizamos las tablas para que apunten al nuevo lugar que ocupa.
			GameObjectIndex objectOfLast = m_componentToObject.at(indexOfLast);
			m_objectToComponent.at(objectOfLast) = compIndex;
			m_componentToObject.at(compIndex) = objectOfLast;

			// Eliminación final.
			m_components.RemoveLast();

			m_objectToComponent.erase(obj);
			m_componentToObject.erase(indexOfLast);
		}


		/// @param obj ID del objeto.
		/// @return Referencia no estable al componente del objeto dado.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		TComponent& GetComponent(GameObjectIndex obj) {
			return m_components.At(m_objectToComponent.at(obj));
		}

		/// @param obj ID del objeto.
		/// @return Referencia no estable al componente del objeto dado.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		const TComponent& GetComponent(GameObjectIndex obj) const {
			return m_components.At(m_objectToComponent.at(obj));
		}
				
		/// @brief Elimina el componente del objeto eliminado.
		/// @param obj ID del objeto.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		void GameObjectDestroyerd(GameObjectIndex obj) override {
			RemoveComponent(obj);
		}

		/// @param obj Objeto a comprobar.
		/// @return True si tiene el componente añadido.
		bool ObjectHasComponent(GameObjectIndex obj) const {
			return m_objectToComponent.contains(obj);
		}

	private:

		/// @brief Array con todos los componentes de un mismo tipo.
		DynamicArray<TComponent> m_components;

		/// @brief Mapa dueño -> componente.
		std::unordered_map<GameObjectIndex, ComponentIndex> m_objectToComponent;
		
		/// @brief Mapa componente -> dueño.
		std::unordered_map<ComponentIndex, GameObjectIndex> m_componentToObject;

	};

}
