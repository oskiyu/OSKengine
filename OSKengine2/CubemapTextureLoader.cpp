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

#include <json.hpp>
#include <stbi_image.h>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void CubemapTextureLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	CubemapTexture* output = (CubemapTexture*)*asset;

	// Asset file.

	nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));
	
	output->SetName(assetInfo["name"]);

	const static std::string facesNames[] = {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};

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

	OwnedPtr<GRAPHICS::GpuImage> image = Engine::GetRenderer()->GetMemoryAllocator()->CreateCubemapImage(Vector2i(width, height).ToVector2ui(), GRAPHICS::GetColorFormat(numChannels), GRAPHICS::GpuImageUsage::SAMPLED | GRAPHICS::GpuImageUsage::TRANSFER_DESTINATION, GRAPHICS::GpuSharedMemoryType::GPU_ONLY);

	Engine::GetRenderer()->UploadCubemapImageToGpu(image.GetPointer(), data, width * height * numChannels * 6, GRAPHICS::GpuImageLayout::SHADER_READ_ONLY);


	output->_SetImage(image);

	delete[] data;
}
