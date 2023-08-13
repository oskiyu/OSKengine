#include "GameObjectTranslator.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

using namespace OSK::NET;
using namespace OSK::ECS;

GameObjectIndex GameObjectTranslator::AddExternalObject(GameObjectIndex externalId) {
	const GameObjectIndex internalId = Engine::GetEcs()->SpawnObject();

	thisToOther[internalId] = externalId;
	otherToThis[externalId] = internalId;
}

void GameObjectTranslator::UnregisterExternalObject(ECS::GameObjectIndex externalId) {
	thisToOther.erase(otherToThis.at(externalId));
	otherToThis.erase(externalId);
}

GameObjectIndex GameObjectTranslator::GetInternalObject(ECS::GameObjectIndex externalId) const {
	return otherToThis.at(externalId);
}

GameObjectIndex GameObjectTranslator::GetExternalObject(ECS::GameObjectIndex internalId) const {
	return thisToOther.at(internalId);
}
