#pragma once

#include "IComponentContainer.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "Component.h"
#include "Memory.h"

namespace OSK::ECS {

	/// <summary>
	/// Contenedor para el dynamic array de componentes del tipo dado.
	/// Se encarga de manejar la introducci�n y eliminaci�n de componentes.
	/// Los componentes se almacenan en memoria cont�nua.
	/// </summary>
	/// <typeparam name="TComponent">Tipo del componente.</typeparam>
	template <typename TComponent> class ComponentContainer : public IComponentContainer {

	public:

		~ComponentContainer() {
			while (!components.IsEmpty())
				components.RemoveLast();
		}

		/// <summary>
		/// A�ade un componente asignado al objeto dado.
		/// </summary>
		/// <param name="obj">Objeto due�o del componente.</param>
		/// <param name="component">Componente a a�adir.</param>
		/// <returns>El componente ya a�adido.</returns>
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			ComponentIndex componentId = components.GetSize();

			objectToComponent.Insert(obj, componentId);
			componentToObject.Insert(componentId, obj);
			components.Insert(component);

			return GetComponent(obj);
		}

		/// <summary>
		/// Elimina el componente cuyo due�o es el dado.
		/// </summary>
		/// <param name="obj">Due�o del componente.</param>
		void RemoveComponent(GameObjectIndex obj) {
			ComponentIndex compIndex = objectToComponent.Get(obj);
			TSize indexOfLast = components.GetSize() - 1;

			// Hacemos que el componente a eliminar est� en la �ltima posici�n.
			// El �ltimo componente se coloca en el hueco del componente eliminado,
			// para evitar dejar huecos en el array.
			MEMORY::MemorySwap(&components[compIndex], &components[indexOfLast], sizeof(TComponent));

			// Obtenemos el objeto due�o del componente que acabamos de colocar
			// donde el componente eliminado.
			// Actualizamos las tablas para que apunten al nuevo lugar que ocupa.
			GameObjectIndex objectOfLast = componentToObject.Get(indexOfLast);
			objectToComponent.Get(objectOfLast) = compIndex;
			componentToObject.Get(compIndex) = objectOfLast;

			// Eliminaci�n final.
			components.RemoveLast();

			objectToComponent.Remove(obj);
			componentToObject.Remove(indexOfLast);
		}

		/// <summary>
		/// Devuelve una referencia al componente del objeto dado.
		/// </summary>
		TComponent& GetComponent(GameObjectIndex obj) const {
			return components.At(objectToComponent.Get(obj));
		}

		/// <summary>
		/// Elimina el componente del objeto eliminado.
		/// </summary>
		void GameObjectDestroyerd(GameObjectIndex obj) override {
			RemoveComponent(obj);
		}

	private:

		/// <summary>
		/// Array con todos los componentes de un mismo tipo.
		/// </summary>
		DynamicArray<TComponent> components;

		/// <summary>
		/// Mapa due�o -> componente.
		/// </summary>
		HashMap<GameObjectIndex, ComponentIndex> objectToComponent;
		/// <summary>
		/// Mapa componente -> due�o.
		/// </summary>
		HashMap<ComponentIndex, GameObjectIndex> componentToObject;

	};

}
