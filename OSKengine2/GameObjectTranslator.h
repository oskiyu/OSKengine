#pragma once

#include "HashMap.hpp"
#include "GameObject.h"

namespace OSK::NET {

	/// @brief 
	class GameObjectTranslator {

	public:

		/// @brief 
		/// @param externalId 
		/// @return 
		ECS::GameObjectIndex AddExternalObject(ECS::GameObjectIndex externalId);

		/// @brief 
		/// @param externalId 
		void UnregisterExternalObject(ECS::GameObjectIndex externalId);


		/// @brief 
		/// @param externalId 
		/// @return 
		ECS::GameObjectIndex GetInternalObject(ECS::GameObjectIndex externalId) const;

		/// @brief 
		/// @param internalId 
		/// @return 
		ECS::GameObjectIndex GetExternalObject(ECS::GameObjectIndex internalId) const;

	private:

		/// @brief ID interno -> ID externo.
		std::unordered_map<ECS::GameObjectIndex, ECS::GameObjectIndex> thisToOther;

		/// @brief ID externo -> ID interno.
		std::unordered_map<ECS::GameObjectIndex, ECS::GameObjectIndex> otherToThis;

	};

}
