module;

#include <OSKengine/Model3D.h>
#include <OSKengine/Mesh3D.h>
#include <OSKengine/MaterialInstance.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/Model3D.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/IMaterialSlot.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/MaterialSystem.h>
#include <OSKengine/Texture.h>
#include <OSKengine/Material.h>
#include <OSKengine/RenderSystem3D.h>

export module PbrModelLoader;

import Scene;

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

export class PbrModelUtils {

public:

	static void LoadMaterial(const std::string& path) {
		pbrMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(path);
	}

	static void SetupPbrModel(ModelComponent3D* modelComponent, Model3D* model) {
		RenderSystem3D* renderSystem = Engine::GetEntityComponentSystem()->GetSystem<RenderSystem3D>();

		modelComponent->SetModel(model);
		modelComponent->SetMaterial(pbrMaterial);

		modelComponent->BindUniformBufferForAllMeshes("global", "camera", Scene::GetCameraBuffer());
		modelComponent->BindTextureForAllMeshes("model", "stexture", Engine::GetAssetManager()->Load<Texture>("Resources/Assets/texture0.json", "GLOBAL"));
		modelComponent->BindGpuImageForAllMeshes("global", "irradianceMap", Scene::GetIrradianceMap()->GetGpuImage());
		modelComponent->BindUniformBufferForAllMeshes("global", "dirLight", renderSystem->GetDirLightUniformBuffer());
		modelComponent->BindUniformBufferForAllMeshes("global", "dirLightShadowMat", renderSystem->GetShadowMap()->GetDirLightMatrixUniformBuffer());

		for (TSize i = 0; i < model->GetMeshes().GetSize(); i++) {
			auto& metadata = model->GetMetadata().meshesMetadata[i];

			const GpuImage* images[3]{};
			for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
				images[i] = renderSystem->GetShadowMap()->GetShadowImage(i);

			modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetGpuImages("dirLightShadowMap", images, SampledChannel::DEPTH);
			modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->FlushUpdate();

			if (metadata.materialTextures.GetSize() > 0) {
				for (auto& texture : metadata.materialTextures)
					modelComponent->GetMeshMaterialInstance(i)->GetSlot("model")->SetGpuImage("stexture", model->GetImage(texture.second));

				modelComponent->GetMeshMaterialInstance(i)->GetSlot("model")->FlushUpdate();
			}
		}
	}

private:

	static Material* pbrMaterial;

};

Material* PbrModelUtils::pbrMaterial = nullptr;
