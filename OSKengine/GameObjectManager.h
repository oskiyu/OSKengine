#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Component.h"

#include <queue>
#include <array>

namespace OSK::ECS {

	//ID de un objeto.
	typedef uint32_t GameObjectID;

	//Máximo de objetos a guardar.
	constexpr GameObjectID MAX_OBJECTS = 10000;

	//Maneja todas las entidades.
	class GameObjectManager {

	public:
				
		GameObjectManager() {
			for (GameObjectID obj = 0; obj < MAX_OBJECTS; obj++) {
				freeObjects.push(obj);
			}
		}

		//Crea un objeto.
		GameObjectID CreateGameObject() {
			livingEnitites++;

			GameObjectID ID = freeObjects.front();
			
			freeObjects.pop();

			return ID;
		}

		//Elimina un objeto.
		void DestroyGameObject(GameObjectID object) {
			livingEnitites--;

			signatures[object].reset();

			freeObjects.push(object);
		}

		//Establece el signature (componentes que tiene).
		void SetSignature(GameObjectID obj, Signature sign) {
			signatures[obj] = sign;
		}

		//Obtiene los tipos de componentes que tiene.
		Signature GetSignature(GameObjectID obj) {
			return signatures[obj];
		}

	private:

		std::queue<GameObjectID> freeObjects;
		std::array<Signature, MAX_OBJECTS> signatures;
		uint32_t livingEnitites = 0;

	};

}