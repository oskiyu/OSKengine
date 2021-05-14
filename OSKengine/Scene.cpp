#include "Scene.h"

#include "SkyboxToken.h"
#include "PlaceToken.h"
#include "VersionToken.h"
#include "TerrainToken.h"

#include "FileIO.h"

using namespace OSK;
using namespace OSK::SceneSystem::Loader;

Scene::Scene(EntityComponentSystem* ecs, RenderSystem3D* renderSystem) {
	entityComponentSystem = ecs;
	this->renderSystem = renderSystem;
}

Scene::~Scene() {
	Unload();
}

void Scene::Load(const std::string path) {
	LoadScript(FileIO::ReadFromFile(path));
}

void Scene::LoadSkybox(const std::string& path) {
	renderSystem->GetRenderScene()->LoadSkybox(path);
}

void Scene::LoadTerrain(const std::string& path, const Vector2f& quadSize, float maxHeight) {
	renderSystem->GetRenderScene()->LoadHeightmap(path, quadSize, maxHeight);
}

GameObject* Scene::GetGameObjectByName(const std::string& instanceName) {
	return objectsByInstanceName[instanceName];
}

void Scene::LoadScript(const std::string& code) {
	Unload();

	auto tokens = sceneLoader.GetTokens(code);

	for (auto i : tokens) {
		auto tokenType = i->tokenType;

		switch (tokenType) {

			case SceneSystem::Loader::TokenType::VERSION: {
				break;
			}
			case SceneSystem::Loader::TokenType::SKYBOX: {
				auto token = (SceneSystem::Loader::SkyboxToken*)i;

				LoadSkybox(token->GetPath());

				break;
			}
			case SceneSystem::Loader::TokenType::TERRAIN: {
				auto token = (SceneSystem::Loader::TerrainToken*)i;

				LoadTerrain(token->GetPath(), { 5 }, 35 );

				break;
			}
			case SceneSystem::Loader::TokenType::PLACE: {
				auto token = (SceneSystem::Loader::PlaceToken*)i;

				Spawn(token->GetClassName(), token->GetInstanceName(), token->GetPosition(), token->GetAxis(), token->GetAngle(), token->GetScale());

				break;
			}

		}
	}
}

GameObject* Scene::Spawn(const std::string& className, const Vector3f& position, const Vector3f& axis, float angle, const Vector3f& size) {
	auto ptr = entityComponentSystem->Spawn(className, position, axis, angle, size);

	ptr->DeleteOnScene = [this](GameObject* obj) {
		objectsOnScene.remove(obj);

		objectsByInstanceName.erase(obj->GetInstanceName());
	};

	objectsOnScene.push_back(ptr);
	objectsByInstanceName[ptr->instanceName] = ptr;

	return ptr;
}

GameObject* Scene::Spawn(const std::string& className, const std::string& instanceName, const Vector3f& position, const Vector3f& axis, float angle, const Vector3f& size) {
	auto ptr = entityComponentSystem->Spawn(className, instanceName, position, axis, angle, size);

	ptr->DeleteOnScene = [this](GameObject* obj) {
		objectsOnScene.remove(obj);

		objectsByInstanceName.erase(obj->GetInstanceName());
	};

	objectsOnScene.push_back(ptr);
	objectsByInstanceName[instanceName] = ptr;

	return ptr;
}

void Scene::Unload() {
	while (objectsOnScene.size() > 0)
		objectsOnScene.begin().operator*()->Remove();
}

