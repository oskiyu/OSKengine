#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "DynamicArray.hpp"

namespace OSK {
	class ICommandList;
}

namespace OSK::ECS{

	class OSKAPI_CALL ISystem {

	public:

		~ISystem() = default;

		virtual void OnCreate();
		virtual void OnTick(TDeltaTime deltaTime);
		virtual void OnRemove();

		const Signature& GetSignature() const;

		void AddObject(GameObjectIndex obj);
		void RemoveObject(GameObjectIndex obj);

	private:

		DynamicArray<GameObjectIndex> processedObjects;
		Signature signature;

	};

}
