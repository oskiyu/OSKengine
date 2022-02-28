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

#define STB_IMAGE_IMPLEMENTATION
#include <stbi_image.h>
#include <json.hpp>

using namespace OSK;

void TextureLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	Texture* output = (Texture*)*asset;

	// Asset file.

	nlohmann::json assetInfo = nlohmann::json::parse(FileIO::ReadFromFile(assetFilePath));
	//OSK_ASSERT()
	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	// Texture.

	int width = 0;
	int height = 0;
	int numChannels = 0;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &width, &height, &numChannels, numChannels);

	output->_SetSize(Vector2ui(width, height));
	output->_SetNumberOfChannels(numChannels);

	Vector2ui size(width, height);
	OwnedPtr<GpuImage> image = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(size, GetColorFormat(numChannels), GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_DESTINATION, GpuSharedMemoryType::GPU_ONLY, true);

	auto stagingBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateStagingBuffer(width * height * numChannels);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(pixels, stagingBuffer->GetSize());
	stagingBuffer->Unmap();

	auto commandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	commandList->RegisterStagingBuffer(stagingBuffer);
	commandList->Start();
	commandList->TransitionImageLayout(image.GetPointer(), GpuImageLayout::TRANSFER_DESTINATION);
	commandList->CopyBufferToImage(stagingBuffer.GetPointer(), image.GetPointer());
	commandList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(commandList.GetPointer());

	stbi_image_free(pixels);

	output->_SetImage(image);
}
