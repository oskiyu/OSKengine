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
	
	cubemapGenRenderTarget.SetTargetImageUsage(GpuImageUsage::TRANSFER_SOURCE);
	cubemapGenRenderTarget.Create(irradianceLayerSize, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);

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
	float npixels[500];
	memcpy(npixels, pixels, sizeof(float) * 500);

	// Crear imagen original y llenarla.
	const Vector3ui size(width, height, 1);
	UniquePtr<GpuImage> originalImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(size, GRAPHICS::GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT, GRAPHICS::GpuImageUsage::TRANSFER_SOURCE | GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION | GRAPHICS::GpuImageUsage::SAMPLED, GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
	Engine::GetRenderer()->UploadImageToGpu(originalImage.GetPointer(), (TByte*)pixels, size.X * size.Y * size.Z * GetFormatNumberOfBytes(Format::RGBA32_SFLOAT), GRAPHICS::GpuImageLayout::SHADER_READ_ONLY);

	// Crear cubemap y renderizarlo.
	OwnedPtr<GpuImage> intermediateCubemap = Engine::GetRenderer()->GetMemoryAllocator()->CreateCubemapImage(irradianceLayerSize, Format::RGBA32_SFLOAT, GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, GpuSharedMemoryType::GPU_ONLY).GetPointer();
	OwnedPtr<GpuImage> finalImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateCubemapImage(irradianceLayerSize, Format::RGBA32_SFLOAT, GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED | GpuImageUsage::CUBEMAP, GpuSharedMemoryType::GPU_ONLY);

	// Material
	cubemapGenMaterialInstance->GetSlot("global")->SetGpuImage("image", originalImage.GetPointer());
	cubemapGenMaterialInstance->GetSlot("global")->FlushUpdate();

	cubemapConvolutionMaterialInstance->GetSlot("global")->SetGpuImage("image", intermediateCubemap.GetPointer());
	cubemapConvolutionMaterialInstance->GetSlot("global")->FlushUpdate();

	// Renderizado
	OwnedPtr<ICommandList> cmdList = Engine::GetRenderer()->CreateSingleUseCommandList().GetPointer();
	cmdList->Start();
	GenCubemap(intermediateCubemap.GetPointer(), cmdList.GetPointer());
	ConvoluteCubemap(finalImage.GetPointer(), cmdList.GetPointer());
	cmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(cmdList.GetPointer());

	output->_SetGpuImage(finalImage);
	output->_SetOriginalCubemap(intermediateCubemap);

	stbi_image_free(pixels);
}

void IrradianceMapLoader::DrawCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList) {
	struct {
		glm::mat4 cameraProjection = IrradianceMapLoader::cameraProjection;
		glm::mat4 cameraView = glm::mat4(1.0f);
	} renderInfo;

	for (TSize i = 0; i < 6; i++) {
		for (TSize mipLevel = 0; mipLevel < targetCubemap->GetMipLevels(); mipLevel++) {
			cmdList->BeginGraphicsRenderpass(&cubemapGenRenderTarget);

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

			cmdList->EndGraphicsRenderpass(&cubemapGenRenderTarget);

			cmdList->TransitionImageLayout(cubemapGenRenderTarget.GetMainTargetImage(Engine::GetRenderer()->GetCurrentFrameIndex()), GpuImageLayout::TRANSFER_SOURCE, 0, 1);
			cmdList->CopyImageToImage(cubemapGenRenderTarget.GetMainTargetImage(Engine::GetRenderer()->GetCurrentFrameIndex()), targetCubemap, 1, 0, i, 0, mipLevel, viewport.rectangle.GetRectangleSize());
		}
	}
}

void IrradianceMapLoader::GenCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList) {
	cmdList->TransitionImageLayout(targetCubemap, GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION, 0, 6);

	cmdList->BindMaterial(cubemapGenMaterial);
	cmdList->BindMaterialSlot(cubemapGenMaterialInstance->GetSlot("global"));

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.X, irradianceLayerSize.Y });

	DrawCubemap(targetCubemap, cmdList);

	cmdList->TransitionImageLayout(targetCubemap, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SHADER_READ_ONLY, 0, 6);
}

void IrradianceMapLoader::ConvoluteCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList) {
	cmdList->TransitionImageLayout(targetCubemap, GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION, 0, 6);

	cmdList->BindMaterial(cubemapConvolutionMaterial);
	cmdList->BindMaterialSlot(cubemapConvolutionMaterialInstance->GetSlot("global"));

	cmdList->SetScissor({ 0, 0, irradianceLayerSize.X, irradianceLayerSize.Y });

	DrawCubemap(targetCubemap, cmdList);

	cmdList->TransitionImageLayout(targetCubemap, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SHADER_READ_ONLY, 0, 6);
}
