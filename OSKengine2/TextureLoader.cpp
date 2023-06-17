#include "TextureLoader.h"

#include "Texture.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuBuffer.h"
#include "ICommandList.h"
#include "FileIO.h"
#include "GpuImageLayout.h"
#include "RenderApiType.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"

// #define STB_IMAGE_IMPLEMENTATION <- está en tiny_glft.h
#include <stbi_image.h>
#include <json.hpp>

#include "InvalidDescriptionFileException.h"

using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

bool HasRgbFormat(OSK::GRAPHICS::RenderApiType api) {
	return false;
}

void TextureLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	Texture* output = (Texture*)*asset;

	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	// Texture.

	int width = 0;
	int height = 0;
	int numChannels = 0;
	stbi_info(texturePath.c_str(), &width, &height, &numChannels);

	// Las gráficas no soportan imágenes de 24 bits de manera nativa.
	if (numChannels != 4 && !HasRgbFormat(Engine::GetRenderer()->GetRenderApi()))
		numChannels = 4;

	stbi_uc* pixels = stbi_load(texturePath.c_str(), &width, &height, nullptr, numChannels);

	output->_SetSize(Vector2ui(width, height));
	output->_SetNumberOfChannels(numChannels);

	const GRAPHICS::Format imageFormat = assetInfo.contains("is_hdr") && assetInfo["is_hdr"] == "true"
		? GRAPHICS::Format::RGBA32_SFLOAT
		: GRAPHICS::Format::RGBA8_SRGB;

	const Vector2ui size(width, height);
	const auto imageInfo = GpuImageCreateInfo::CreateDefault2D(size, imageFormat, GRAPHICS::GpuImageUsage::SAMPLED | GRAPHICS::GpuImageUsage::TRANSFER_SOURCE | GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION);
	auto image = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo);
	image->SetDebugName(assetInfo["name"]);

	OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList();
	uploadCmdList->Reset();
	uploadCmdList->Start();

	uploadCmdList->SetGpuImageBarrier(
		image.GetPointer(), 
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	Engine::GetRenderer()->UploadImageToGpu(
		image.GetPointer(), 
		pixels, 
		image->GetNumberOfBytes(), 
		uploadCmdList.GetPointer());

	uploadCmdList->SetGpuImageBarrier(
		image.GetPointer(), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ));

	uploadCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(uploadCmdList.GetPointer());

	stbi_image_free(pixels);

	output->_SetImage(image);
}
