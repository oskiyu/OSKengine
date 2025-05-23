#include "IrradianceMapLoader.h"

#include "IrradianceMap.h"
#include "FileIO.h"
#include "Format.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"
#include "RenderTarget.h"
#include "ICommandList.h"

#include "MaterialSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "IMaterialSlot.h"

#include "Viewport.h"
#include "Model3D.h"

#include "CopyImageInfo.h"

#include <json.hpp>
#include <stbi_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


glm::mat4 IrradianceMapLoader::cameraProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 IrradianceMapLoader::cameraViews[6] = {
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
};


IrradianceMapLoader::IrradianceMapLoader() {
	cubemapGenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/AssetsGen/material_irradiance_gen.json");
	cubemapConvolutionMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/AssetsGen/material_irradiance_convolution.json");

	cubemapGenMaterialInstance = cubemapGenMaterial->CreateInstance();
	cubemapConvolutionMaterialInstance = cubemapConvolutionMaterial->CreateInstance();

	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA16_SFLOAT, .usage = GpuImageUsage::TRANSFER_SOURCE, .name = "Irradiance Map Color Target"};
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM , .name = "Irradiance Map Depth Target" };
	cubemapGenRenderTarget.Create(irradianceLayerSize, { colorInfo }, depthInfo);

	cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/Models/cube.json");
}

void IrradianceMapLoader::Load(const std::string& assetFilePath, IrradianceMap* asset) {
	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string texturePath = assetInfo["raw_asset_path"];
	asset->SetName(assetInfo["name"]);

	// Original texture
	int width = 0;
	int height = 0;
	int numChannels = 0;

	float* pixels = stbi_loadf(texturePath.c_str(), &width, &height, &numChannels, 4);

	// Crear imagen original y llenarla.
	const Vector3ui size(width, height, 1);
	const Vector2ui size2D(width, height);

	const GpuImageUsage imageUsage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED;
	GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(size2D, Format::RGBA32_SFLOAT, imageUsage);
	
	// Imagen cargada desde disco (en formato 2D).
	UniquePtr<GpuImage> originalImage = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo);
	originalImage->SetDebugName("Irradiance Map Original Image");

	UniquePtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);
	uploadCmdList->Reset();
	uploadCmdList->Start();

	uploadCmdList->SetGpuImageBarrier(
		originalImage.GetPointer(), 
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	Engine::GetRenderer()->UploadImageToGpu(originalImage.GetPointer(), (TByte*)pixels, static_cast<USize64>(size.x) * size.y * size.z * GetFormatNumberOfBytes(Format::RGBA32_SFLOAT), uploadCmdList.GetPointer());

	uploadCmdList->SetGpuImageBarrier(
		originalImage.GetPointer(), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE));

	uploadCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(std::move(uploadCmdList));

	// Crear cubemap y renderizarlo.
	UniquePtr<GpuImage> intermediateCubemap = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(
		irradianceLayerSize, 
		Format::RGBA16_SFLOAT, 
		GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, 
		GpuSharedMemoryType::GPU_ONLY,
		GpuQueueType::MAIN);

	UniquePtr<GpuImage> finalImage = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(
		irradianceLayerSize, 
		Format::RGBA16_SFLOAT, 
		GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, 
		GpuSharedMemoryType::GPU_ONLY,
		GpuQueueType::MAIN);

	// Material
	cubemapGenMaterialInstance->GetSlot("global")->SetGpuImage(
		"image", 
		*originalImage->GetView(GpuImageViewConfig::CreateSampled_Default()),
		GpuImageSamplerDesc::CreateDefault_WithMipMap(*originalImage));
	cubemapGenMaterialInstance->GetSlot("global")->FlushUpdate();

	cubemapConvolutionMaterialInstance->GetSlot("global")->SetGpuImage(
		"image", 
		*intermediateCubemap->GetView(GpuImageViewConfig::CreateSampled_Cubemap()),
		GpuImageSamplerDesc::CreateDefault_WithMipMap(*intermediateCubemap));
	cubemapConvolutionMaterialInstance->GetSlot("global")->FlushUpdate();

	// Renderizado
	UniquePtr<ICommandList> cmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);
	cmdList->Reset();
	cmdList->Start();

	GenCubemap(intermediateCubemap.GetPointer(), cmdList.GetPointer());

	cmdList->SetGpuImageBarrier(
		intermediateCubemap.GetPointer(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{ .baseLayer = 0, .numLayers = 6, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	ConvoluteCubemap(finalImage.GetPointer(), cmdList.GetPointer());

	cmdList->SetGpuImageBarrier(
		finalImage.GetPointer(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{ .baseLayer = 0, .numLayers = 6, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	cmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(std::move(cmdList));

	asset->_SetGpuImage(std::move(finalImage));
	asset->_SetOriginalCubemap(std::move(intermediateCubemap));

	stbi_image_free(pixels);
}

void IrradianceMapLoader::DrawCubemap(GpuImage* targetCubemap, ICommandList* cmdList, Material* material, IMaterialSlot* materialSlot) {
	struct {
		glm::mat4 cameraProjection = IrradianceMapLoader::cameraProjection;
		glm::mat4 cameraView = glm::mat4(1.0f);
	} renderInfo;

	for (UIndex32 i = 0; i < 6; i++) {
		for (UIndex32 mipLevel = 0; mipLevel < targetCubemap->GetMipLevels(); mipLevel++) {
			cmdList->BeginGraphicsRenderpass(&cubemapGenRenderTarget);

			cmdList->BindMaterial(*material);
			cmdList->BindMaterialSlot(*materialSlot);

			Viewport viewport{};
			viewport.rectangle = {
				0,
				0,
				static_cast<uint32_t>(static_cast<float>(irradianceLayerSize.x) * glm::pow(0.5f, mipLevel)),
				static_cast<uint32_t>(static_cast<float>(irradianceLayerSize.y) * glm::pow(0.5f, mipLevel))
			};

			cmdList->SetViewport(viewport);

			renderInfo.cameraView = cameraViews[i];
			cmdList->PushMaterialConstants("info", renderInfo);

			cmdList->BindVertexBuffer(cubemapModel->GetModel().GetVertexBuffer());
			cmdList->BindIndexBuffer(cubemapModel->GetModel().GetIndexBuffer());
			cmdList->DrawSingleInstance(cubemapModel->GetModel().GetTotalIndexCount());

			cmdList->EndGraphicsRenderpass();

			cmdList->SetGpuImageBarrier(
				cubemapGenRenderTarget.GetMainColorImage(), 
				GpuImageLayout::TRANSFER_SOURCE,
				GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE), 
				GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
				GpuImageRange{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

			CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(viewport.rectangle.GetRectangleSize());
			copyInfo.destinationArrayLevel = i;
			copyInfo.destinationMipLevel = mipLevel;

			cmdList->RawCopyImageToImage(*cubemapGenRenderTarget.GetMainColorImage(), targetCubemap, copyInfo);

			/*cmdList->CopyImageToImage(
				*cubemapGenRenderTarget.GetMainColorImage(),
				targetCubemap,
				copyInfo,
				GpuImageFilteringType::NEAREST);*/
		}
	}
}

void IrradianceMapLoader::GenCubemap(GpuImage* targetCubemap, ICommandList* cmdList) {
	cmdList->SetGpuImageBarrier(
		targetCubemap, 
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.x, irradianceLayerSize.y });

	DrawCubemap(targetCubemap, cmdList, cubemapGenMaterial, cubemapGenMaterialInstance->GetSlot("global"));
}

void IrradianceMapLoader::ConvoluteCubemap(GpuImage* targetCubemap, ICommandList* cmdList) {
	// El render target cubemap se copiar� a la imagen.
	cmdList->SetGpuImageBarrier(
		targetCubemap,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.x, irradianceLayerSize.y });

	DrawCubemap(targetCubemap, cmdList, cubemapConvolutionMaterial, cubemapConvolutionMaterialInstance->GetSlot("global"));
}
