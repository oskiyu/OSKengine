#include "TextureLoader.h"

#include "Texture.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "IGpuDataBuffer.h"
#include "ICommandList.h"
#include "FileIO.h"
#include "GpuImageLayout.h"
#include "RenderApiType.h"

// #define STB_IMAGE_IMPLEMENTATION <- está en tiny_glft.h
#include <stbi_image.h>
#include <json.hpp>

using namespace OSK::ASSETS;

bool HasRgbFormat(OSK::GRAPHICS::RenderApiType api) {
	return false;
}

void TextureLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	Texture* output = (Texture*)*asset;

	// Asset file.

	nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));
	//OSK_ASSERT()
	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	// Texture.

	int width = 0;
	int height = 0;
	int numChannels = 0;
	stbi_info(texturePath.c_str(), &width, &height, &numChannels);

	// Las gráficas no soportan imágenes de 24 bits de manera nativa.
	if (numChannels == 3 && !HasRgbFormat(Engine::GetRenderer()->GetRenderApi()))
		numChannels = 4;

	stbi_uc* pixels = stbi_load(texturePath.c_str(), &width, &height, nullptr, numChannels);

	output->_SetSize(Vector2ui(width, height));
	output->_SetNumberOfChannels(numChannels);

	Vector2ui size(width, height);
	OwnedPtr<GRAPHICS::GpuImage> image = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(size, GRAPHICS::GetColorFormat(numChannels), GRAPHICS::GpuImageUsage::SAMPLED | GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION, GRAPHICS::GpuSharedMemoryType::GPU_ONLY, true);

	Engine::GetRenderer()->UploadImageToGpu(image.GetPointer(), pixels, width * height * numChannels, GRAPHICS::GpuImageLayout::SHADER_READ_ONLY);
	stbi_image_free(pixels);

	output->_SetImage(image);
}
