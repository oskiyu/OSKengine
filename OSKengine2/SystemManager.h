#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "GameObject.h"
#include "Component.h"

#include <string>

namespace OSK {
	class ICommandList;
}

namespace OSK::ECS {

	class ISystem;

	class OSKAPI_CALL SystemManager {

	public:

		void OnTick(TDeltaTime deltaTime);

		void GameObjectDestroyed(GameObjectIndex obj);
		void GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature);

		template <typename TSystem> TSystem* RegisterSystem() {
			TSystem* sistema = new TSystem;

			sistemas.Insert(TSystem::GetSystemName(), (ISystem*)sistema);

			return sistema;
		}

	private:

		HashMap<std::string, OwnedPtr<ISystem>> sistemas;

	};

}
