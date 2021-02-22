#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "GameObjectManager.h"

#include <array>
#include <unordered_map>

namespace OSK::ECS {

	//Clase base de un array de componentes.
	class IComponentArray {

	public:

		virtual ~IComponentArray() = default;

		virtual void GameObjectDestroyed(GameObjectID obj) = 0;

	};


	//Array de componentes de un tipo.
	template <typename T> class ComponentArray : public IComponentArray {

	public:

		//Añade un componente al array, perteneciente al objeto obj.
		void InsertData(GameObjectID obj, T component) {
			size_t newID = size;

			objectToID[obj] = newID;
			idToObject[newID] = obj;
			componentArray[newID] = component;

			size++;
		}

		//Elimina el componente del objeto.
		void RemoveData(GameObjectID obj) {
			size_t id = objectToID[obj];
			size_t lastID = size - 1;

			componentArray[id] = componentArray[lastID];

			GameObjectID objOfLastElem = idToObject[lastID];
			objectToID[objOfLastElem] = id;
			idToObject[id] = objOfLastElem;

			objectToID.erase(obj);
			idToObject.erase(lastID);

			size--;
		}

		//Obtiene el componente de un objeto.
		T& GetData(GameObjectID obj) {
			return componentArray[objectToID[obj]];
		}

		//Destruye el componente del objeto, si existe.
		void GameObjectDestroyed(GameObjectID obj) override {
			if (objectToID.find(obj) != objectToID.end()) {
				RemoveData(obj);
			}
		}

	private:

		std::array<T, MAX_OBJECTS> componentArray;
		std::unordered_map<GameObjectID, size_t> objectToID;
		std::unordered_map<size_t, GameObjectID> idToObject;

		size_t size = 0;

	};
}