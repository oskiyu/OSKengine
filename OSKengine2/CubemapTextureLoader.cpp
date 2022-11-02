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

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void CubemapTextureLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	CubemapTexture* output = (CubemapTexture*)*asset;

	// Asset file.

	nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));

	OSK_ASSERT(assetInfo.contains("file_type"), "Archivo de cubemap incorrecto: no se encuentra 'file_type'.");
	OSK_ASSERT(assetInfo.contains("spec_ver"), "Archivo de cubemap incorrecto: no se encuentra 'spec_ver'.");
	OSK_ASSERT(assetInfo.contains("name"), "Archivo de cubemap incorrecto: no se encuentra 'name'.");
	OSK_ASSERT(assetInfo.contains("asset_type"), "Archivo de cubemap incorrecto: no se encuentra 'asset_type'.");
	OSK_ASSERT(assetInfo.contains("faces_files"), "Archivo de cubemap incorrecto: no se encuentra 'faces_files'.");

	output->SetName(assetInfo["name"]);

	const static std::string facesNames[] = {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};

	for (const auto& i : facesNames) {
		OSK_ASSERT(assetInfo["faces_files"].contains(i), "Archivo de cubemap incorrecto: no existe '" + i + "'.");
	}

	// Texture.

	int width = 0;
	int height = 0;
	int numChannels = 0;
	stbi_info(((std::string)assetInfo["faces_files"][facesNames[0]]).c_str(), &width, &height, &numChannels);

	// Las gráficas no soportan imágenes de 24 bits de manera nativa.
	if (numChannels == 3)
		numChannels = 4;

	TByte* data = new TByte[width * height * 4 * 6];

	for (TSize i = 0; i < 6; i++) {
		std::string facePath = assetInfo["faces_files"][facesNames[i]];
		OSK_ASSERT(IO::FileIO::FileExists(facePath), "No existe la textura " + facePath + ".");

		stbi_uc* pixels = stbi_load(facePath.c_str(), &width, &height, nullptr, 4);
		memcpy(data + i * width * height * 4, pixels, width * height * 4);
		stbi_image_free(pixels);
	}

	OwnedPtr<GRAPHICS::GpuImage> image = Engine::GetRenderer()->GetAllocator()->CreateCubemapImage(Vector2i(width, height).ToVector2ui(), GRAPHICS::Format::RGBA8_SRGB, GRAPHICS::GpuImageUsage::SAMPLED | GRAPHICS::GpuImageUsage::TRANSFER_SOURCE| GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION, GRAPHICS::GpuSharedMemoryType::GPU_ONLY);

	OwnedPtr<ICommandList> copyCmdList = Engine::GetRenderer()->CreateSingleUseCommandList();
	copyCmdList->Reset();
	copyCmdList->Start();

	copyCmdList->SetGpuImageBarrier(image.GetPointer(), GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Engine::GetRenderer()->UploadCubemapImageToGpu(image.GetPointer(), data, width * height * numChannels * 6, copyCmdList.GetPointer());

	copyCmdList->SetGpuImageBarrier(image.GetPointer(), GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	copyCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(copyCmdList.GetPointer());

	output->_SetImage(image);

	delete[] data;
}
