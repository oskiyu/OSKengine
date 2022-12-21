// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "SpecularMapLoader.h"

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

#include <stbi_image.h>

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

SpecularMapLoader::SpecularMapLoader() {
	cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/cube.json", "OSK::IrradianceMapLoader");
}

SpecularMapLoader::~SpecularMapLoader() {
	Engine::GetAssetManager()->DeleteLifetime("OSK::IrradianceMapLoader");
}

void SpecularMapLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	// Asset file.
	nlohmann::json assetInfo = nlohmann::json::parse(FileIO::ReadFromFile(assetFilePath));

	OSK_ASSERT(assetInfo.contains("file_type"), "Archivo de textura incorrecto: no se encuentra 'file_type'.");
	OSK_ASSERT(assetInfo.contains("spec_ver"), "Archivo de textura incorrecto: no se encuentra 'spec_ver'.");
	OSK_ASSERT(assetInfo.contains("name"), "Archivo de textura incorrecto: no se encuentra 'name'.");
	OSK_ASSERT(assetInfo.contains("asset_type"), "Archivo de textura incorrecto: no se encuentra 'asset_type'.");
	OSK_ASSERT(assetInfo.contains("raw_asset_path"), "Archivo de textura incorrecto: no se encuentra 'raw_asset_path'.");

	std::string texturePath = assetInfo["raw_asset_path"];

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

	UploadImage(originalImage.GetPointer(), pixels, { originalImageSize.X, originalImageSize.Y, 1 });

	// Creación del cubemap.
	// Cada mip level representa el reflejo para una rugosidad específica.
	const TSize maxMipLevel = 5;

	GpuImageSamplerDesc sampler{};
	sampler.mipMapMode = GpuImageMipmapMode::CUSTOM;
	sampler.minMipLevel = 0;
	sampler.maxMipLevel = maxMipLevel;
	sampler.filteringType = GpuImageFilteringType::LIENAR;

	OwnedPtr<GpuImage> targetCubemap = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(maxResolution, 
		Format::RGBA32_SFLOAT, GpuImageUsage::COLOR | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, 
		GpuSharedMemoryType::GPU_ONLY, sampler);

	// TODO: renderizar a cubemap

	// Preprocesado
	OwnedPtr<ICommandList> cmdList = Engine::GetRenderer()->CreateSingleUseCommandList();

	for (TIndex mipLevel = 0; mipLevel < maxMipLevel; mipLevel++) {
		const float roughness = (float)mipLevel / (maxMipLevel - 1.0f);
		DrawPreFilter(targetCubemap.GetPointer(), cmdList.GetPointer(), mipLevel, roughness);
	}
}

void SpecularMapLoader::DrawPreFilter(GpuImage* cubemap, ICommandList* cmdList, TIndex mipLevel, float roughness) {
	struct {
		glm::mat4 cameraProjection = SpecularMapLoader::cameraProjection;
		glm::mat4 cameraView = glm::mat4(1.0f);
	} renderInfo;

	for (TIndex faceId = 0; faceId < 6; faceId++) {
		cmdList->BeginGraphicsRenderpass(&cubemapRenderTarget);

		const float sizeRatio = glm::pow(0.5f, mipLevel);
		const Viewport viewport{
			.rectangle = {
				0, 0,
				(uint32_t)(cubemapRenderTarget.GetSize().X * sizeRatio),
				(uint32_t)(cubemapRenderTarget.GetSize().Y * sizeRatio)
			}
		};

		cmdList->SetViewport(viewport);
		cmdList->BindMaterial(prefilterMaterial);
		cmdList->BindMaterialSlot(prefilterMaterialInstance->GetSlot("global"));

		renderInfo.cameraView = cameraViews[faceId];
		cmdList->PushMaterialConstants("roughness", roughness);

		cmdList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
		cmdList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
		cmdList->DrawSingleInstance(cubemapModel->GetIndexCount());

		cmdList->EndGraphicsRenderpass();

		const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

		cmdList->SetGpuImageBarrier(cubemapRenderTarget.GetMainColorImage(resourceIndex),
			GpuImageLayout::TRANSFER_SOURCE,
			GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE),
			GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = mipLevel, .numMipLevels = 1 });

		CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(viewport.rectangle.GetRectangleSize());
		copyInfo.destinationArrayLevel = faceId;
		copyInfo.destinationMipLevel = mipLevel;
		cmdList->CopyImageToImage(cubemapRenderTarget.GetMainColorImage(resourceIndex), cubemap, copyInfo);
	}
}

void SpecularMapLoader::UploadImage(GpuImage* img, const float* pixels, const Vector3ui& size) const {
	OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList();
	uploadCmdList->Reset();
	uploadCmdList->Start();

	uploadCmdList->SetGpuImageBarrier(img, GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Engine::GetRenderer()->UploadImageToGpu(img, (TByte*)pixels, size.X * size.Y * size.Z * GetFormatNumberOfBytes(Format::RGBA32_SFLOAT), uploadCmdList.GetPointer());

	uploadCmdList->SetGpuImageBarrier(img, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

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
