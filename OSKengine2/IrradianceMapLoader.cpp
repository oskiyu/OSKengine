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

	cubemapGenMaterialInstance = cubemapGenMaterial->CreateInstance().GetPointer();
	cubemapConvolutionMaterialInstance = cubemapConvolutionMaterial->CreateInstance().GetPointer();

	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA32_SFLOAT, .usage = GpuImageUsage::TRANSFER_SOURCE, .name = "Irradiance Map Color Target"};
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM , .name = "Irradiance Map Depth Target" };
	cubemapGenRenderTarget.Create(irradianceLayerSize, { colorInfo }, depthInfo);

	cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/Models/cube.json", "OSK::IrradianceMapLoader");
}

IrradianceMapLoader::~IrradianceMapLoader() {
	Engine::GetAssetManager()->DeleteLifetime("OSK::IrradianceMapLoader");
}

void IrradianceMapLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	IrradianceMap* output = (IrradianceMap*)*asset;

	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

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
	UniquePtr<GpuImage> originalImage = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo).GetPointer();
	originalImage->SetDebugName("Irradiance Map Original Image");

	OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList().GetPointer();
	uploadCmdList->Reset();
	uploadCmdList->Start();

	uploadCmdList->SetGpuImageBarrier(
		originalImage.GetPointer(), 
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	Engine::GetRenderer()->UploadImageToGpu(originalImage.GetPointer(), (TByte*)pixels, size.x * size.y * size.Z * GetFormatNumberOfBytes(Format::RGBA32_SFLOAT), uploadCmdList.GetPointer());

	uploadCmdList->SetGpuImageBarrier(
		originalImage.GetPointer(), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE));

	uploadCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(uploadCmdList.GetPointer());

	// Crear cubemap y renderizarlo.
	OwnedPtr<GpuImage> intermediateCubemap = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(irradianceLayerSize, Format::RGBA32_SFLOAT, GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, GpuSharedMemoryType::GPU_ONLY).GetPointer();
	OwnedPtr<GpuImage> finalImage = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(
		irradianceLayerSize, 
		Format::RGBA32_SFLOAT, 
		GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, 
		GpuSharedMemoryType::GPU_ONLY);

	// Material
	cubemapGenMaterialInstance->GetSlot("global")->SetGpuImage("image", originalImage->GetView(GpuImageViewConfig::CreateSampled_Default()));
	cubemapGenMaterialInstance->GetSlot("global")->FlushUpdate();

	cubemapConvolutionMaterialInstance->GetSlot("global")->SetGpuImage("image", intermediateCubemap->GetView(GpuImageViewConfig::CreateSampled_Cubemap()));
	cubemapConvolutionMaterialInstance->GetSlot("global")->FlushUpdate();

	// Renderizado
	OwnedPtr<ICommandList> cmdList = Engine::GetRenderer()->CreateSingleUseCommandList().GetPointer();
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
	Engine::GetRenderer()->SubmitSingleUseCommandList(cmdList);

	output->_SetGpuImage(finalImage);
	output->_SetOriginalCubemap(intermediateCubemap);

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

			cmdList->BindVertexBuffer(*cubemapModel->GetVertexBuffer());
			cmdList->BindIndexBuffer(*cubemapModel->GetIndexBuffer());
			cmdList->DrawSingleInstance(cubemapModel->GetIndexCount());

			cmdList->EndGraphicsRenderpass();

			cmdList->SetGpuImageBarrier(
				cubemapGenRenderTarget.GetMainColorImage(Engine::GetRenderer()->GetCurrentFrameIndex()), 
				GpuImageLayout::TRANSFER_SOURCE,
				GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE), 
				GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
				GpuImageRange{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

			CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(viewport.rectangle.GetRectangleSize());
			copyInfo.destinationArrayLevel = i;
			copyInfo.destinationMipLevel = mipLevel;
			cmdList->RawCopyImageToImage(*cubemapGenRenderTarget.GetMainColorImage(Engine::GetRenderer()->GetCurrentFrameIndex()), targetCubemap, copyInfo);
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
	// El render target cubemap se copiará a la imagen.
	cmdList->SetGpuImageBarrier(
		targetCubemap,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.x, irradianceLayerSize.y });

	DrawCubemap(targetCubemap, cmdList, cubemapConvolutionMaterial, cubemapConvolutionMaterialInstance->GetSlot("global"));
}
