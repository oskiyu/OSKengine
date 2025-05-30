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

// #define STB_IMAGE_IMPLEMENTATION <- est� en tiny_glft.h
#include <stbi_image.h>
#include <json.hpp>

#include "InvalidDescriptionFileException.h"

using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

bool HasRgbFormat(OSK::GRAPHICS::RenderApiType) {
	return false;
}

void TextureLoader::Load(const std::string& assetFilePath, Texture* asset) {
	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string texturePath = assetInfo["raw_asset_path"];
	asset->SetName(assetInfo["name"]);

	// Texture.

	int width = 0;
	int height = 0;
	int numChannels = 0;
	stbi_info(texturePath.c_str(), &width, &height, &numChannels);

	// Las gr�ficas no soportan im�genes de 24 bits de manera nativa.
	if (numChannels != 4 && !HasRgbFormat(Engine::GetRenderer()->GetRenderApi()))
		numChannels = 4;

	stbi_uc* pixels = stbi_load(texturePath.c_str(), &width, &height, nullptr, numChannels);

	asset->_SetSize(Vector2ui(width, height));
	asset->_SetNumberOfChannels(numChannels);

	const GRAPHICS::Format imageFormat = assetInfo.contains("is_hdr") && assetInfo["is_hdr"] == "true"
		? GRAPHICS::Format::RGBA32_SFLOAT
		: GRAPHICS::Format::RGBA8_UNORM;

	const Vector2ui size(width, height);
	const auto imageInfo = GpuImageCreateInfo::CreateDefault2D(size, imageFormat, GRAPHICS::GpuImageUsage::SAMPLED | GRAPHICS::GpuImageUsage::TRANSFER_SOURCE | GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION);
	auto image = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo);
	image->SetDebugName(assetInfo["name"]);

	// Preferir cola de transferencia: NO, por mipmap generation.
	UniquePtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);
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

	uploadCmdList->TransferToQueue(
		image.GetPointer(),
		*Engine::GetRenderer()->GetMainRenderingQueue());

	uploadCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(std::move(uploadCmdList));

	stbi_image_free(pixels);

	asset->_SetImage(std::move(image));
}

void TextureLoader::RegisterTexture(AssetOwningRef<Texture>&& texture) {
	m_externallyLoadedTextures.Insert(std::move(texture));
}
