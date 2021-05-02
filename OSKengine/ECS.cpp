#include "ECS.h"

#include "GameObject.h"

using namespace OSK;

EntityComponentSystem::~EntityComponentSystem() {
	for (auto& i : GameObjects) {
		i.second->Remove();
	}
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
	GameObject* out = RegisteredGOClasses[className]();
	out->ID = CreateGameObject();
	out->ECSsystem = this;
	out->OnCreate();

	out->Delete = [this](GameObject* obj) {
		DestroyGameObject(obj->ID);

		if (GameObjects.find(obj->ID) != GameObjects.end()) {
			GameObject* toDelete = GameObjects[obj->ID];
			GameObjects.erase(obj->ID);
			delete toDelete;
		}
	};

	GameObjects[out->ID] = out;

	return out;
}

GameObject* EntityComponentSystem::GetGameObjectByID(ECS::GameObjectID id) const {
	return GameObjects.find(id).operator*().second;
}

void EntityComponentSystem::RegisterGameObjectClass(const std::string& className, GameObjectCreateFunc func) {
	RegisteredGOClasses[className] = func;
}