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

#include <json.hpp>
#include <stbi_image.h>
#include <glm.hpp>
#include <ext/matrix_clip_space.hpp>
#include <gtc/matrix_transform.hpp>

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
	cubemapGenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_irradiance_gen.json");
	cubemapConvolutionMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_irradiance_convolution.json");

	cubemapGenMaterialInstance = cubemapGenMaterial->CreateInstance().GetPointer();
	cubemapConvolutionMaterialInstance = cubemapConvolutionMaterial->CreateInstance().GetPointer();

	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA32_SFLOAT, .usage = GpuImageUsage::TRANSFER_SOURCE };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT };
	cubemapGenRenderTarget.Create(irradianceLayerSize, { colorInfo }, depthInfo);

	cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/cube.json", "OSK::IrradianceMapLoader");
}

IrradianceMapLoader::~IrradianceMapLoader() {
	Engine::GetAssetManager()->DeleteLifetime("OSK::IrradianceMapLoader");
}

void IrradianceMapLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	IrradianceMap* output = (IrradianceMap*)*asset;

	// Asset file.
	nlohmann::json assetInfo = nlohmann::json::parse(FileIO::ReadFromFile(assetFilePath));

	OSK_ASSERT(assetInfo.contains("file_type"), "Archivo de textura incorrecto: no se encuentra 'file_type'.");
	OSK_ASSERT(assetInfo.contains("spec_ver"), "Archivo de textura incorrecto: no se encuentra 'spec_ver'.");
	OSK_ASSERT(assetInfo.contains("name"), "Archivo de textura incorrecto: no se encuentra 'name'.");
	OSK_ASSERT(assetInfo.contains("asset_type"), "Archivo de textura incorrecto: no se encuentra 'asset_type'.");
	OSK_ASSERT(assetInfo.contains("raw_asset_path"), "Archivo de textura incorrecto: no se encuentra 'raw_asset_path'.");
	
	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	// Original texture
	int width = 0;
	int height = 0;
	int numChannels = 0;

	float* pixels = stbi_loadf(texturePath.c_str(), &width, &height, &numChannels, 4);

	// Crear imagen original y llenarla.
	const Vector3ui size(width, height, 1);
	UniquePtr<GpuImage> originalImage = Engine::GetRenderer()->GetAllocator()->CreateImage(size, GRAPHICS::GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT, GRAPHICS::GpuImageUsage::TRANSFER_SOURCE | GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION | GRAPHICS::GpuImageUsage::SAMPLED, GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();

	OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList().GetPointer();
	uploadCmdList->Reset();
	uploadCmdList->Start();

	uploadCmdList->SetGpuImageBarrier(originalImage.GetPointer(), GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Engine::GetRenderer()->UploadImageToGpu(originalImage.GetPointer(), (TByte*)pixels, size.X * size.Y * size.Z * GetFormatNumberOfBytes(Format::RGBA32_SFLOAT), uploadCmdList.GetPointer());

	uploadCmdList->SetGpuImageBarrier(originalImage.GetPointer(), GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	uploadCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(uploadCmdList.GetPointer());

	// Crear cubemap y renderizarlo.
	OwnedPtr<GpuImage> intermediateCubemap = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(irradianceLayerSize, Format::RGBA32_SFLOAT, GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, GpuSharedMemoryType::GPU_ONLY).GetPointer();
	OwnedPtr<GpuImage> finalImage = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(irradianceLayerSize, Format::RGBA32_SFLOAT, GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, GpuSharedMemoryType::GPU_ONLY);

	// Material
	cubemapGenMaterialInstance->GetSlot("global")->SetGpuImage("image", originalImage.GetPointer());
	cubemapGenMaterialInstance->GetSlot("global")->FlushUpdate();

	cubemapConvolutionMaterialInstance->GetSlot("global")->SetGpuImage("image", intermediateCubemap.GetPointer());
	cubemapConvolutionMaterialInstance->GetSlot("global")->FlushUpdate();

	// Renderizado
	OwnedPtr<ICommandList> cmdList = Engine::GetRenderer()->CreateSingleUseCommandList().GetPointer();
	cmdList->Reset();
	cmdList->Start();
	GenCubemap(intermediateCubemap.GetPointer(), cmdList.GetPointer());
	ConvoluteCubemap(finalImage.GetPointer(), cmdList.GetPointer());
	cmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(cmdList.GetPointer());

	output->_SetGpuImage(finalImage);
	output->_SetOriginalCubemap(intermediateCubemap);

	stbi_image_free(pixels);
}

void IrradianceMapLoader::DrawCubemap(GpuImage* targetCubemap, ICommandList* cmdList, Material* material, IMaterialSlot* materialSlot) {
	struct {
		glm::mat4 cameraProjection = IrradianceMapLoader::cameraProjection;
		glm::mat4 cameraView = glm::mat4(1.0f);
	} renderInfo;

	for (TSize i = 0; i < 6; i++) {
		for (TSize mipLevel = 0; mipLevel < targetCubemap->GetMipLevels(); mipLevel++) {
			cmdList->BeginGraphicsRenderpass(&cubemapGenRenderTarget);

			cmdList->BindMaterial(material);
			cmdList->BindMaterialSlot(materialSlot);

			Viewport viewport{};
			viewport.rectangle = {
				0,
				0,
				static_cast<uint32_t>(static_cast<float>(irradianceLayerSize.X) * glm::pow(0.5f, mipLevel)),
				static_cast<uint32_t>(static_cast<float>(irradianceLayerSize.Y) * glm::pow(0.5f, mipLevel))
			};

			cmdList->SetViewport(viewport);

			renderInfo.cameraView = cameraViews[i];
			cmdList->PushMaterialConstants("info", renderInfo);

			cmdList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
			cmdList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
			cmdList->DrawSingleInstance(cubemapModel->GetIndexCount());

			cmdList->EndGraphicsRenderpass();

			cmdList->SetGpuImageBarrier(cubemapGenRenderTarget.GetMainColorImage(Engine::GetRenderer()->GetCurrentFrameIndex()), GpuImageLayout::TRANSFER_SOURCE,
				GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ),
				GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

			cmdList->CopyImageToImage(cubemapGenRenderTarget.GetMainColorImage(Engine::GetRenderer()->GetCurrentFrameIndex()), targetCubemap, 1, 0, i, 0, mipLevel, viewport.rectangle.GetRectangleSize());
		}
	}
}

void IrradianceMapLoader::GenCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList) {
	cmdList->SetGpuImageBarrier(targetCubemap, GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = 6, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.X, irradianceLayerSize.Y });

	DrawCubemap(targetCubemap, cmdList, cubemapGenMaterial, cubemapGenMaterialInstance->GetSlot("global"));

	cmdList->SetGpuImageBarrier(targetCubemap, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = 6, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void IrradianceMapLoader::ConvoluteCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList) {
	cmdList->SetGpuImageBarrier(targetCubemap, GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = 6, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.X, irradianceLayerSize.Y });

	DrawCubemap(targetCubemap, cmdList, cubemapConvolutionMaterial, cubemapConvolutionMaterialInstance->GetSlot("global"));

	cmdList->SetGpuImageBarrier(targetCubemap, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = 6, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}
