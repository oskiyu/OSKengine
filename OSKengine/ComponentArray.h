#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "IComponentArray.h"
#include "GameObjectManager.h"

#include <array>
#include <unordered_map>

namespace OSK::ECS {

	/// <summary>
	/// Array de componentes de un tipo.
	/// Contiene todos los componentes de un tipo de las entidades.
	/// </summary>
	/// <typeparam name="T">Componente.</typeparam>
	template <typename T> class ComponentArray : public IComponentArray {

	public:

		/// <summary>
		/// Añade un componente al array, perteneciente al objeto obj.
		/// </summary>
		/// <param name="obj">ID de la entidad dueña.</param>
		/// <param name="component">Componente.</param>
		T& InsertData(GameObjectID obj, T component) {
			size_t newID = size;

			objectToID[obj] = newID;
			idToObject[newID] = obj;
			componentArray[newID] = component;

			size++;

			return componentArray[newID];
		}

		/// <summary>
		/// Elimina el componente del objeto.
		/// </summary>
		/// <param name="obj">ID del objeto.</param>
		void RemoveData(GameObjectID obj) {
			const size_t id = objectToID[obj];
			const size_t lastID = size - 1;

			componentArray[id] = componentArray[lastID];

			GameObjectID objOfLastElem = idToObject[lastID];
			objectToID[objOfLastElem] = id;
			idToObject[id] = objOfLastElem;

			objectToID.erase(obj);
			idToObject.erase(lastID);

			size--;
		}

		/// <summary>
		/// Obtiene el componente de un objeto.
		/// </summary>
		/// <param name="obj">ID del objeto.</param>
		/// <returns>Componente del objeto.</returns>
		T& GetData(GameObjectID obj) {
			return componentArray[objectToID[obj]];
		}

		/// <summary>
		/// Destruye el componente del objeto, si existe.
		/// </summary>
		/// <param name="obj">ID del objeto destruido.</param>
		void GameObjectDestroyed(GameObjectID obj) override {
			if (objectToID.find(obj) != objectToID.end()) {
				RemoveData(obj);
			}
		}

	private:

		/// <summary>
		/// Array de los componentes.
		/// </summary>
		std::array<T, MAX_OBJECTS> componentArray;

		/// <summary>
		/// ID del componente, a partir de un objeto.
		/// </summary>
		std::unordered_map<GameObjectID, size_t> objectToID;

		/// <summary>
		/// ID del objeto, a partir del componente.
		/// </summary>
		std::unordered_map<size_t, GameObjectID> idToObject;

		/// <summary>
		/// Componentes almacenados.
		/// </summary>
		size_t size = 0;

	};

}