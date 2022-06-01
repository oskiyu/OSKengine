module;

// Local includes
#include <OSKengine/Material.h>
#include <OSKengine/MaterialInstance.h>
#include <OSKengine/CubemapTexture.h>
#include <OSKengine/Model3D.h>
#include <OSKengine/Mesh3D.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/CubemapTextureLoader.h>
#include <OSKengine/ModelLoader3D.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/MaterialSystem.h>
#include <OSKengine/IMaterialSlot.h>
#include <OSKengine/ICommandList.h>
#include <OSKengine/IGpuUniformBuffer.h>
#include <OSKengine/UniquePtr.hpp>

export module Skybox;

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

#include <OSKengine/IGpuUniformBuffer.h>

export import <string>;

export class Skybox {

public:

	void Load(const std::string& textureAssetPath, const std::string& modelPath, const std::string& lifetimePool) {
		cubemap = Engine::GetAssetManager()->Load<CubemapTexture>(textureAssetPath, lifetimePool);
		model = Engine::GetAssetManager()->Load<Model3D>(modelPath, lifetimePool);

		material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
		materialInstance = material->CreateInstance().GetPointer();
	}

	void SetCameraBuffer(const IGpuUniformBuffer& cameraBuffer) {
		materialInstance->GetSlot("global")->SetUniformBuffer("camera", &cameraBuffer);
		materialInstance->GetSlot("global")->SetGpuImage("cubemap", cubemap->GetGpuImage());
		materialInstance->GetSlot("global")->FlushUpdate();
	}

	void Render(ICommandList* commandList) {
		commandList->BindMaterial(material);
		commandList->BindMaterialSlot(materialInstance->GetSlot("global"));
		commandList->BindVertexBuffer(model->GetVertexBuffer());
		commandList->BindIndexBuffer(model->GetIndexBuffer());
		commandList->DrawSingleInstance(model->GetIndexCount());
	}
	
private:

	Material* material = nullptr;
	UniquePtr<MaterialInstance> materialInstance;

	CubemapTexture* cubemap = nullptr;
	Model3D* model = nullptr;

};
