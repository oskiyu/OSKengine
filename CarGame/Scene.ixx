module;

#include <OSKengine/IGpuUniformBuffer.h>
#include <OSKengine/GameObject.h>
#include <OSKengine/EntityComponentSystem.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/CameraComponent3D.h>
#include <OSKengine/Transform3D.h>
#include <OSKengine/IGpuMemoryAllocator.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/Vector4.hpp>
#include <OSKengine/ICommandList.h>
#include <OSKengine/Logger.h>
#include <glm.hpp>
#include <OSKengine/UniquePtr.hpp>
#include <OSKengine/TerrainComponent.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/Material.h>
#include <OSKengine/MaterialSystem.h>
#include <OSKengine/IMaterialSlot.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/Texture.h>
#include <OSKengine/Color.hpp>

export module Scene;

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

export import Skybox;
export import PointLight;

export class Scene {

public:

	static void Create() {
		cameraBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(Vector4f)).GetPointer();

		// Valores por defecto, para que se vea algo por pantalla aunque no haya ninguna cámara activa.
		cameraBuffer->MapMemory();
		cameraBuffer->Write(glm::mat4(1.0f));
		cameraBuffer->Write(glm::mat4(1.0f));
		cameraBuffer->Write(glm::vec3(1.0f));
		cameraBuffer->Unmap();

		// Terreno
		terrainObject = Engine::GetEntityComponentSystem()->SpawnObject();
		auto& terrainComponent = Engine::GetEntityComponentSystem()->AddComponent<TerrainComponent>(terrainObject, {});
		auto& terrainTransform = Engine::GetEntityComponentSystem()->AddComponent<Transform3D>(terrainObject, { terrainObject });
		terrainComponent.Generate({ 100 });

		terrainMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_terrain.json");

		terrainComponent.SetMaterialInstance(terrainMaterial->CreateInstance());

		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", cameraBuffer.GetPointer());
		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetTexture("heightmap",
			Engine::GetAssetManager()->Load<Texture>("Resources/Assets/heightmap0.json", "GLOBAL"));
		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetTexture("texture",
			Engine::GetAssetManager()->Load<Texture>("Resources/Assets/terrain0.json", "GLOBAL"));
		terrainComponent.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

		terrainTransform.SetScale({ 200, 5, 120 });

		// Lights
		directionalLight.direction = { 1.0f, -3.f, 0.0f };
		directionalLight.intensity = 0.8f;
		directionalLight.color = Color(253 / 255.f, 253 / 255.f, 225 / 255.f);

		lightsBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(directionalLight)).GetPointer();
		lightsBuffer->MapMemory();
		lightsBuffer->Write(directionalLight);
		lightsBuffer->Unmap();
	}

	static void Delete() {
		cameraBuffer.Delete();
		lightsBuffer.Delete();
	}

	static void LoadSkybox(const std::string& textureAssetPath, const std::string& modelPath, const std::string& lifetimePool) {
		skybox.Load(textureAssetPath, modelPath, lifetimePool);
		skybox.SetCameraBuffer(cameraBuffer.GetValue());
	}

	static inline Skybox& GetSkybox() {
		return skybox;
	}

	static void SetCameraObject(GameObjectIndex inCameraObject) {
		cameraObject = inCameraObject;
	}

	static GameObjectIndex GetCameraObject() {
		return cameraObject;
	}

	static void UpdateCameraBuffer() {
		if (cameraObject == EMPTY_GAME_OBJECT)
			return;
		if (!Engine::GetEntityComponentSystem()->ObjectHasComponent<CameraComponent3D>(cameraObject)) {
			cameraObject = EMPTY_GAME_OBJECT;
			Engine::GetLogger()->Log(IO::LogLevel::WARNING, "El objeto de la cámara activa ya no es válido (no tiene componente de cámara).");
			return;
		}
		if (!Engine::GetEntityComponentSystem()->ObjectHasComponent<Transform3D>(cameraObject)) {
			cameraObject = EMPTY_GAME_OBJECT;
			Engine::GetLogger()->Log(IO::LogLevel::WARNING, "El objeto de la cámara activa ya no es válido (no tiene componente de transform).");
			return;
		}

		auto& cameraComponent = Engine::GetEntityComponentSystem()->GetComponent<CameraComponent3D>(cameraObject);
		auto& transformComponent = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(cameraObject);

		cameraComponent.UpdateTransform(&transformComponent);

		cameraBuffer->MapMemory();
		cameraBuffer->Write(cameraComponent.GetProjectionMatrix(transformComponent));
		cameraBuffer->Write(cameraComponent.GetViewMatrix());
		cameraBuffer->Write(transformComponent.GetPosition());
		cameraBuffer->Unmap();
	}

	static IGpuUniformBuffer* GetCameraBuffer() {
		return cameraBuffer.GetPointer();
	}

	static IGpuUniformBuffer* GetLightsBuffer() {
		return lightsBuffer.GetPointer();
	}

private:

	static Skybox skybox;
	static GameObjectIndex terrainObject;
	static Material* terrainMaterial;

	static DirectionalLight directionalLight;

	// Cámara activa.
	static GameObjectIndex cameraObject;
	static UniquePtr<IGpuUniformBuffer> cameraBuffer;
	static UniquePtr<IGpuUniformBuffer> lightsBuffer;

};

GameObjectIndex Scene::cameraObject = EMPTY_GAME_OBJECT;
UniquePtr<IGpuUniformBuffer> Scene::cameraBuffer = nullptr;
Skybox Scene::skybox;

GameObjectIndex Scene::terrainObject = EMPTY_GAME_OBJECT;
Material* Scene::terrainMaterial = nullptr;

DirectionalLight Scene::directionalLight = {};
UniquePtr<IGpuUniformBuffer> Scene::lightsBuffer = nullptr;
