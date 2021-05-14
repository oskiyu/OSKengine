#include "ECS.h"

#include "GameObject.h"

#include "Scanner.h"
#include "FileIO.h"

#include "PlaceToken.h"
#include "SkyboxToken.h"
#include "VersionToken.h"
#include "TerrainToken.h"

using namespace OSK;
using namespace OSK::SceneSystem::Loader;

EntityComponentSystem::~EntityComponentSystem() {
	while (gameObjects.size() > 0)
		gameObjects.begin().operator*().second->Remove();
}

EntityComponentSystem::EntityComponentSystem() {
	systemManager = new ECS::SystemManager();
	componentManager = new ECS::ComponentManager();
	objectManager = new ECS::GameObjectManager();
}

void EntityComponentSystem::OnTick(deltaTime_t deltaTime) {
	systemManager->OnTick(deltaTime);
}

void EntityComponentSystem::OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
	systemManager->OnDraw(cmdBuffer, i);
}

ECS::GameObjectID EntityComponentSystem::CreateGameObject() {
	return objectManager->CreateGameObject();
}

void EntityComponentSystem::DestroyGameObject(ECS::GameObjectID object) {
	systemManager->GameObjectDestroyed(object);
	componentManager->GameObjectDestroyed(object);
	objectManager->DestroyGameObject(object);
}

GameObject* EntityComponentSystem::Spawn(const std::string& className, const Vector3f& position, const Vector3f& axis, float angle, const Vector3f& size) {
	return Spawn(className, "unnamed", position, axis, angle, size);
}

GameObject* EntityComponentSystem::Spawn(const std::string& className, const std::string& instanceName, const Vector3f& position, const Vector3f& axis, float angle, const Vector3f& size) {
	GameObject* out = registeredGameObjectClasses[className]();
	out->ID = CreateGameObject();
	out->entityComponentSystem = this;
	out->instanceName = instanceName;
	out->GetTransform()->SetPosition(position);
	out->GetTransform()->RotateWorldSpace(angle, axis);
	out->GetTransform()->SetScale(size);
	out->OnCreate();

	out->Delete = [this](GameObject* obj) {
		DestroyGameObject(obj->ID);
		std::string name = obj->GetInstanceName();

		gameObjectsReferences.remove(obj);

		if (instancesByName.find(name) != instancesByName.end())
			instancesByName.erase(name);

		if (gameObjects.find(obj->ID) != gameObjects.end()) {
			GameObject* toDelete = gameObjects[obj->ID];
			gameObjects.erase(obj->ID);
			delete toDelete;
		}
	};

	gameObjects[out->ID] = out;
	gameObjectsReferences.push_back(out);

	instancesByName[instanceName] = out->ID;

	return out;
}

GameObject* EntityComponentSystem::GetGameObjectByID(ECS::GameObjectID id) const {
	return gameObjects.find(id).operator*().second;
}

void EntityComponentSystem::RegisterGameObjectClass(const std::string& className, GameObjectCreateFunc func) {
	registeredGameObjectClasses[className] = func;
}
