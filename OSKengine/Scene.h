#pragma once

#include "OSKmacros.h"

#include <string>
#include <unordered_map>
#include <typeindex>

namespace OSK {

	class GameObjectReg {

	public:

		std::unordered_map<std::string, std::type_index> RegisteredClasses;

		template <typename T> void Register(const std::string& className) {
			RegisteredClasses[className] = typeid(T);
		}

	};

	struct RegisteredGameObject {

		std::string ClassName = "";

	};

	class Scene {

	public:

		std::unordered_map<std::string, RegisteredGameObject> RegisteredObjects;

	};

}
