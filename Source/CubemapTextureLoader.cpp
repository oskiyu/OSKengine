#include "CubemapTextureLoader.h"

#include "CubemapTexture.h"
#include "FileIO.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"
#include "GpuImageUsage.h"
#include "Format.h"

#include <json.hpp>
#include <stbi_image.h>

#include "RawAssetFileNotFoundException.h"
#include "InvalidDescriptionFileException.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void CubemapTextureLoader::Load(const std::string& assetFilePath, CubemapTexture* asset) {
	// Asset file.
	const nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));
	
	OSK_ASSERT(assetInfo.contains("file_type"), InvalidDescriptionFileException("No se encuentra file_type", assetFilePath));
	OSK_ASSERT(assetInfo.contains("spec_ver"), InvalidDescriptionFileException("No se encuentra spec_ver", assetFilePath));
	OSK_ASSERT(assetInfo.contains("name"), InvalidDescriptionFileException("No se encuentra name", assetFilePath));
	OSK_ASSERT(assetInfo.contains("asset_type"), InvalidDescriptionFileException("No se encuentra asset_type", assetFilePath));

	asset->SetName(assetInfo["name"]);

	const std::array<std::string, 6> facesNames = {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};

	for (const auto& faceName : facesNames)
		OSK_ASSERT(assetInfo["faces_files"].contains(faceName), RawAssetFileNotFoundException(faceName));

	// Texture.

	int width = 0;
	int height = 0;
	int numChannels = 0;
	const auto& faceName = assetInfo["faces_files"][facesNames[0]];
	stbi_info(((std::string)faceName).c_str(), &width, &height, &numChannels);

	// Las gráficas no soportan imágenes de 24 bits de manera nativa.
	if (numChannels == 3)
		numChannels = 4;

	TByte* data = new TByte[width * height * 4 * 6];

	for (USize32 i = 0; i < 6; i++) {
		std::string facePath = assetInfo["faces_files"][facesNames[i]];
		OSK_ASSERT(IO::FileIO::FileExists(facePath), RawAssetFileNotFoundException(facePath));

		stbi_uc* pixels = stbi_load(facePath.c_str(), &width, &height, nullptr, 4);

		const USize64 faceSize = width * height * numChannels;
		const USize64 offsetPerFace = faceSize;
		const USize64 offset = i * offsetPerFace;
		memcpy(data + offset, pixels, faceSize);
		
		stbi_image_free(pixels);
	}

	OwnedPtr<GRAPHICS::GpuImage> image = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(
		Vector2i(width, height).ToVector2ui(), 
		GRAPHICS::Format::RGBA8_SRGB, 
		GRAPHICS::GpuImageUsage::SAMPLED | GRAPHICS::GpuImageUsage::TRANSFER_SOURCE| GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION, 
		GRAPHICS::GpuSharedMemoryType::GPU_ONLY,
		GRAPHICS::GpuQueueType::MAIN);

	OwnedPtr<ICommandList> copyCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);
	copyCmdList->Reset();
	copyCmdList->Start();

	copyCmdList->SetGpuImageBarrier(
		image.GetPointer(), 
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
		GpuImageRange{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Engine::GetRenderer()->UploadCubemapImageToGpu(image.GetPointer(), data, width * height * numChannels * 6, copyCmdList.GetPointer());

	copyCmdList->SetGpuImageBarrier(
		image.GetPointer(), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ),
		GpuImageRange{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	
	copyCmdList->TransferToQueue(
		image.GetPointer(),
		*Engine::GetRenderer()->GetMainRenderingQueue());

	copyCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(copyCmdList.GetPointer());

	asset->_SetImage(image);

	delete[] data;
}
