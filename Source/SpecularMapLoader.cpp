// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "SpecularMapLoader.h"

#include "SpecularMap.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "FileIO.h"

#include "GpuImageDimensions.h"
#include "GpuImageLayout.h"
#include "GpuMemoryTypes.h"
#include "Viewport.h"
#include "Model3D.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "CopyImageInfo.h"

#include "Assert.h"

#include <stbi_image.h>

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

SpecularMapLoader::SpecularMapLoader() {
	generationMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/AssetsGen/material_specular_gen.json");
	prefilterMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/AssetsGen/material_specular_prefilter.json");
	lutGenerationMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/AssetsGen/material_specular_lut_gen.json");

	generationMaterialInstance = generationMaterial->CreateInstance().GetPointer();
	prefilterMaterialInstance = prefilterMaterial->CreateInstance().GetPointer();
	lutGenerationMaterialInstance = lutGenerationMaterial->CreateInstance().GetPointer();

	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA16_SFLOAT, .usage = GpuImageUsage::TRANSFER_SOURCE, .name = "Cubemap Render Color" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM, .name = "Cubemap Depth Color" };
	cubemapRenderTarget.Create(maxResolution, { colorInfo }, depthInfo);

	RenderTargetAttachmentInfo lutInfo{ .format = Format::RG16_SFLOAT, .usage = GpuImageUsage::TRANSFER_SOURCE, .name = "Specular Look-Up Table Generation" };
	lookUpTable.Create(maxResolution, lutInfo);

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateStorage_Default();
	const IGpuImageView* lutGenImage = lookUpTable.GetTargetImage()->GetView(viewConfig);

	lutGenerationMaterialInstance->GetSlot("global")->SetStorageImage("finalImage", lutGenImage);
	lutGenerationMaterialInstance->GetSlot("global")->FlushUpdate();

	cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/Models/cube.json");
}

void SpecularMapLoader::Load(const std::string& assetFilePath, SpecularMap* asset) {
	// Asset file.
	nlohmann::json assetInfo;
	try {
		assetInfo = ValidateDescriptionFile(assetFilePath);
	}
	catch (const EngineException& e) { throw e; }


	std::string texturePath = assetInfo["raw_asset_path"];
	asset->SetName(assetInfo["name"]);

	// Original texture loading
	int width = 0;
	int height = 0;
	int numChannels = 0;

	const float* pixels = stbi_loadf(texturePath.c_str(), &width, &height, &numChannels, 4);

	const Vector2ui originalImageSize = {
		(uint32_t)width,
		(uint32_t)height
	};


	// Carga de la imagen HDR original
	const GpuImageUsage originalImageUsage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED;
	const GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(originalImageSize, Format::RGBA32_SFLOAT, originalImageUsage);
	UniquePtr<GpuImage> originalImage = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo).GetPointer();
	originalImage->SetDebugName("Original Specular 2D");

	UploadImage(originalImage.GetPointer(), pixels, { originalImageSize.x, originalImageSize.y, 1 });

	// Creación del cubemap.
	// Cada mip level representa el reflejo para una rugosidad específica.
	const UIndex32 maxMipLevel = 5;

	GpuImageSamplerDesc origianlSampler{};
	origianlSampler.mipMapMode = GpuImageMipmapMode::NONE;

	GpuImageSamplerDesc prefilterSampler{};
	prefilterSampler.mipMapMode = GpuImageMipmapMode::CUSTOM;
	prefilterSampler.minMipLevel = 0;
	prefilterSampler.maxMipLevel = maxMipLevel;
	prefilterSampler.filteringType = GpuImageFilteringType::LIENAR;

	UniquePtr<GpuImage> originalCubemap = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(
		maxResolution,
		Format::RGBA16_SFLOAT, 
		GpuImageUsage::COLOR | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP | GpuImageUsage::TRANSFER_DESTINATION,
		GpuSharedMemoryType::GPU_ONLY,
		GpuQueueType::MAIN,
		origianlSampler).GetPointer();

	OwnedPtr<GpuImage> targetCubemap = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(
		maxResolution,
		Format::RGBA16_SFLOAT, 
		GpuImageUsage::COLOR | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP | GpuImageUsage::TRANSFER_DESTINATION,
		GpuSharedMemoryType::GPU_ONLY, 
		GpuQueueType::MAIN,
		prefilterSampler);

	originalCubemap->SetDebugName("Original Specular Cubemap");
	targetCubemap->SetDebugName("Prefiltered Specular Cubemap");

	generationMaterialInstance->GetSlot("global")->SetGpuImage("image", originalImage->GetView(GpuImageViewConfig::CreateSampled_SingleMipLevel(0)));
	prefilterMaterialInstance->GetSlot("global")->SetGpuImage("image", originalCubemap->GetView(GpuImageViewConfig::CreateSampled_Cubemap()));

	generationMaterialInstance->GetSlot("global")->FlushUpdate();
	prefilterMaterialInstance->GetSlot("global")->FlushUpdate();

	// Renderizar a cubemap
	OwnedPtr<ICommandList> originalDrawCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);

	originalDrawCmdList->SetDebugName("Specular Cubemap Cmd List");

	originalDrawCmdList->Reset();
	originalDrawCmdList->Start();

	DrawOriginal(originalCubemap.GetPointer(), originalDrawCmdList.GetPointer());

	originalDrawCmdList->Close();

	Engine::GetRenderer()->SubmitSingleUseCommandList(originalDrawCmdList);

	// Preprocesado

	OwnedPtr<ICommandList> prefilterCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);

	prefilterCmdList->SetDebugName("Prefilter and LUT generation");

	prefilterCmdList->Reset();
	prefilterCmdList->Start();

	for (UIndex32 mipLevel = 0; mipLevel < maxMipLevel; mipLevel++) {
		const float roughness = (float)mipLevel / (maxMipLevel - 1.0f);
		DrawPreFilter(targetCubemap.GetPointer(), prefilterCmdList.GetPointer(), mipLevel, roughness);
	}

	// LUT
	GenerateLut(prefilterCmdList.GetPointer());
	GpuImageCreateInfo lutCreateInfo = GpuImageCreateInfo::CreateDefault2D(
		lookUpTable.GetSize(), 
		lookUpTable.GetTargetImage()->GetFormat(), 
		GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_DESTINATION);
	OwnedPtr<GpuImage> lut = Engine::GetRenderer()->GetAllocator()->CreateImage(lutCreateInfo);
	lut->SetDebugName("Specular Look-Up Table");

	prefilterCmdList->SetGpuImageBarrier(lut.GetPointer(), GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(lookUpTable.GetSize());
	prefilterCmdList->RawCopyImageToImage(*lookUpTable.GetTargetImage(), lut.GetPointer(), copyInfo);

	prefilterCmdList->SetGpuImageBarrier(lut.GetPointer(), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));

	prefilterCmdList->Close();

	Engine::GetRenderer()->SubmitSingleUseCommandList(prefilterCmdList);

	asset->_SetCubemapImage(targetCubemap);
	asset->_SetLookUpTable(lut);
}

void SpecularMapLoader::DrawOriginal(GRAPHICS::GpuImage* cubemap, GRAPHICS::ICommandList* cmdList) {
	struct {
		glm::mat4 cameraProjection = SpecularMapLoader::cameraProjection;
		glm::mat4 cameraView = glm::mat4(1.0f);
	} renderInfo;

	for (UIndex32 faceId = 0; faceId < 6; faceId++) {
		cmdList->BeginGraphicsRenderpass(&cubemapRenderTarget);

		cmdList->BindMaterial(*generationMaterial);
		cmdList->BindMaterialSlot(*generationMaterialInstance->GetSlot("global"));

		const Viewport viewport{
			.rectangle = {
				0, 0,
				(uint32_t)(cubemapRenderTarget.GetSize().x),
				(uint32_t)(cubemapRenderTarget.GetSize().y)
			}
		};

		cmdList->SetViewport(viewport);
		cmdList->SetScissor(viewport.rectangle);

		renderInfo.cameraView = cameraViews[faceId];
		cmdList->PushMaterialConstants("info", renderInfo);

		cmdList->BindVertexBuffer(cubemapModel->GetModel().GetVertexBuffer());
		cmdList->BindIndexBuffer(cubemapModel->GetModel().GetIndexBuffer());
		cmdList->DrawSingleInstance(cubemapModel->GetModel().GetTotalIndexCount());

		cmdList->EndGraphicsRenderpass();

		cmdList->SetGpuImageBarrier(cubemapRenderTarget.GetMainColorImage(),
			GpuImageLayout::TRANSFER_SOURCE,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
			GpuImageRange{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

		cmdList->SetGpuImageBarrier(cubemap,
			GpuImageLayout::TRANSFER_DESTINATION,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
			GpuImageRange{ .baseLayer = faceId, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

		CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(viewport.rectangle.GetRectangleSize());
		copyInfo.destinationArrayLevel = faceId;
		cmdList->RawCopyImageToImage(*cubemapRenderTarget.GetMainColorImage(), cubemap, copyInfo);

		cmdList->SetGpuImageBarrier(cubemap,
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ),
			GpuImageRange{ .baseLayer = faceId, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	}
}

void SpecularMapLoader::DrawPreFilter(GpuImage* cubemap, ICommandList* cmdList, UIndex32 mipLevel, float roughness) {
	struct {
		glm::mat4 cameraProjection = SpecularMapLoader::cameraProjection;
		glm::mat4 cameraView = glm::mat4(1.0f);
		float roughness;
	} renderInfo { .roughness = roughness };

	for (UIndex32 faceId = 0; faceId < 6; faceId++) {
		cmdList->BeginGraphicsRenderpass(&cubemapRenderTarget);

		cmdList->BindMaterial(*prefilterMaterial);
		cmdList->BindMaterialSlot(*prefilterMaterialInstance->GetSlot("global"));

		const float sizeRatio = (float)glm::pow(0.5f, mipLevel);
		const Viewport viewport{
			.rectangle = {
				0, 0,
				(uint32_t)(cubemapRenderTarget.GetSize().x * sizeRatio),
				(uint32_t)(cubemapRenderTarget.GetSize().y * sizeRatio)
			}
		};

		cmdList->SetViewport(viewport);
		cmdList->SetScissor(viewport.rectangle);

		renderInfo.cameraView = cameraViews[faceId];
		cmdList->PushMaterialConstants("info", renderInfo);

		cmdList->BindVertexBuffer(cubemapModel->GetModel().GetVertexBuffer());
		cmdList->BindIndexBuffer(cubemapModel->GetModel().GetIndexBuffer());
		cmdList->DrawSingleInstance(cubemapModel->GetModel().GetTotalIndexCount());

		cmdList->EndGraphicsRenderpass();


		cmdList->SetGpuImageBarrier(cubemapRenderTarget.GetMainColorImage(),
			GpuImageLayout::TRANSFER_SOURCE,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
			GpuImageRange{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

		cmdList->SetGpuImageBarrier(cubemap,
			GpuImageLayout::TRANSFER_DESTINATION,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
			GpuImageRange{ .baseLayer = faceId, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

		CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(viewport.rectangle.GetRectangleSize());
		copyInfo.destinationArrayLevel = faceId;
		copyInfo.destinationMipLevel = mipLevel;

		cmdList->RawCopyImageToImage(*cubemapRenderTarget.GetMainColorImage(), cubemap, copyInfo);

		/*cmdList->CopyImageToImage(
			*cubemapRenderTarget.GetMainColorImage(), 
			cubemap, 
			copyInfo,
			GpuImageFilteringType::NEAREST);*/

		cmdList->SetGpuImageBarrier(cubemap,
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ),
			GpuImageRange{ .baseLayer = faceId, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	}
}

void SpecularMapLoader::GenerateLut(ICommandList* cmdList) {
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	GpuImage* img = lookUpTable.GetTargetImage();

	cmdList->SetGpuImageBarrier(img, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	cmdList->BindMaterial(*lutGenerationMaterial);
	cmdList->BindMaterialSlot(*lutGenerationMaterialInstance->GetSlot("global"));

	const Vector2ui imageResolution = lookUpTable.GetSize();
	const Vector2ui threadBlockSize = { 8u, 8u };
	const Vector2ui dispatchResolution = imageResolution / threadBlockSize;
	
	cmdList->DispatchCompute({ dispatchResolution.x, dispatchResolution.y, 1 });

	cmdList->SetGpuImageBarrier(img, GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ));
}

void SpecularMapLoader::UploadImage(GpuImage* img, const float* pixels, const Vector3ui& size) const {
	OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);

	uploadCmdList->Reset();
	uploadCmdList->Start();

	uploadCmdList->SetGpuImageBarrier(
		img, 
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
		GpuImageRange{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Engine::GetRenderer()->UploadImageToGpu(img, (TByte*)pixels, size.x * size.y * size.z * GetFormatNumberOfBytes(Format::RGBA32_SFLOAT), uploadCmdList.GetPointer());

	uploadCmdList->SetGpuImageBarrier(
		img, 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ),
		GpuImageRange{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	uploadCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(uploadCmdList);
}

glm::mat4 SpecularMapLoader::cameraProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 SpecularMapLoader::cameraViews[6] = {
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
};
