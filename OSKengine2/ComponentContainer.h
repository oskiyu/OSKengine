#pragma once

#include "IComponentContainer.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "Component.h"
#include "Memory.h"

namespace OSK::ECS {
		
	/// @brief Contenedor para el dynamic array de componentes del tipo dado.
	/// Se encarga de manejar la introducci�n y eliminaci�n de componentes.
	/// Los componentes se almacenan en memoria cont�nua.
	/// @tparam TComponent Tipo del componente.
	/// 
	/// @pre TComponent debe validar IsEcsComponent.
	template <typename TComponent> requires IsEcsComponent<TComponent>
	class ComponentContainer : public IComponentContainer {

	public:

		~ComponentContainer() override = default;

		/// @brief A�ade un componente asignado al objeto dado.
		/// @param obj Objeto due�o del componente.
		/// @param component Componente a a�adir.
		/// @return Referencia no estable al componente a�adido.
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			ComponentIndex componentId = m_components.GetSize();

			m_objectToComponent[obj] = componentId;
			m_componentToObject[componentId] = obj;
			m_components.Insert(component);

			return GetComponent(obj);
		}

		/// @brief A�ade un componente asignado al objeto dado.
		/// @param obj Objeto due�o del componente.
		/// @param component Componente a a�adir.
		/// @return Referencia no estable al componente a�adido.
		TComponent& AddComponentMove(GameObjectIndex obj, TComponent&& component) {
			const ComponentIndex componentId = m_components.GetSize();

			m_objectToComponent[obj] = componentId;
			m_componentToObject[componentId] = obj;
			m_components.Insert(std::move(component));

			return GetComponent(obj);
		}
				
		/// @brief Elimina el componente cuyo due�o es el dado.
		/// @param obj Due�o del componente.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		void RemoveComponent(GameObjectIndex obj) {
			ComponentIndex compIndex = m_objectToComponent.at(obj);
			const UIndex64 indexOfLast = m_components.GetSize() - 1;

			// Hacemos que el componente a eliminar est� en la �ltima posici�n.
			// El �ltimo componente se coloca en el hueco del componente eliminado,
			// para evitar dejar huecos en el array.
			MEMORY::MemorySwap(&m_components[compIndex], &m_components[indexOfLast], sizeof(TComponent));

			// Obtenemos el objeto due�o del componente que acabamos de colocar
			// donde el componente eliminado.
			// Actualizamos las tablas para que apunten al nuevo lugar que ocupa.
			GameObjectIndex objectOfLast = m_componentToObject.at(indexOfLast);
			m_objectToComponent.at(objectOfLast) = compIndex;
			m_componentToObject.at(compIndex) = objectOfLast;

			// Eliminaci�n final.
			m_components.RemoveLast();

			m_objectToComponent.erase(obj);
			m_componentToObject.erase(indexOfLast);
		}

		/// @brief Devuelve una 
		/// @param obj ID del objeto.
		/// @return Referencia no estable al componente del objeto dado.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		TComponent& GetComponent(GameObjectIndex obj) {
			return m_components.At(m_objectToComponent.at(obj));
		}

		/// @brief Devuelve una 
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

	private:

		/// @brief Array con todos los componentes de un mismo tipo.
		DynamicArray<TComponent> m_components;

		/// @brief Mapa due�o -> componente.
		std::unordered_map<GameObjectIndex, ComponentIndex> m_objectToComponent;
		
		/// @brief Mapa componente -> due�o.
		std::unordered_map<ComponentIndex, GameObjectIndex> m_componentToObject;

	};

}
