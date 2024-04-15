#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "UniquePtr.hpp"

#include "FontLoadingExceptions.h"
#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Font::Font(const std::string& assetFile) : IAsset(assetFile) {

}

void Font::_SetFontFilePath(const std::string& rawFile) {
	m_fontFile = rawFile;
}

void Font::LoadSizedFont(USize32 fontSize) {
	if (ContainsInstance(fontSize))
		return;

	// Carga de la librería.
	static FT_Library freeType = nullptr;
	if (freeType == nullptr) {
		const auto result = FT_Init_FreeType(&freeType);
		OSK_ASSERT(result == 0, FontLibraryInitializationException(result));
	}

	// Un FT_Face describe la tipografía de la fuente.
	FT_Face face = nullptr;

	// Carga de la fuente.
	FT_Error result = FT_New_Face(freeType, m_fontFile.c_str(), 0, &face);
	OSK_ASSERT(result == 0, FontLodaingException(result));

	// Establece el tamaño de esta instancia en concreto.
	FT_Set_Pixel_Sizes(face, 0, fontSize);


	// Definirá cada uno de los caracteres.
	struct FtChar {
		UniquePtr<TByte> data;
		USize32 sizeX = 0;
		USize32 sizeY = 0;
		USize32 left = 0;
		USize32 top = 0;
		USize32 advanceX = 0;
	};

	std::array<FtChar, 255> ftCharacters;

	Vector2ui gpuImageSize = { 0, 0 };

	// Cargamos todos los caracteres.
	// Actualizamos el tamaño de la imagen.
	for (USize32 i = 0; i < 255; i++) {
		// Carga / renderizado del char.
		result = FT_Load_Char(face, i, FT_LOAD_RENDER);
		OSK_ASSERT(!result, FontCharacterLodaingException(i, result));

		ftCharacters[i].sizeX = face->glyph->bitmap.width;
		ftCharacters[i].sizeY = face->glyph->bitmap.rows;

		ftCharacters[i].left = face->glyph->bitmap_left;
		ftCharacters[i].top = face->glyph->bitmap_top;

		ftCharacters[i].advanceX = face->glyph->advance.x;

		ftCharacters[i].data = new TByte[ftCharacters[i].sizeX * ftCharacters[i].sizeY];

		// Copiamos los datos al buffer de la imagen de la GPU.
		const size_t bitmapSize = ftCharacters[i].sizeX * ftCharacters[i].sizeY * sizeof(TByte);
		memcpy(
			ftCharacters[i].data.GetPointer(), 
			face->glyph->bitmap.buffer, 
			bitmapSize);

		// Tamaño de la imagen.
		gpuImageSize.x += ftCharacters[i].sizeX;
		if (ftCharacters[i].sizeY > gpuImageSize.y)
			gpuImageSize.y = ftCharacters[i].sizeY;
	}

	// Creación de la imagen.
	GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(
		gpuImageSize, Format::RGBA8_SRGB, 
		GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION);

	OwnedPtr<GpuImage> gpuImage = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo);
	gpuImage->SetDebugName(std::format("Font {} size {}", GetName(), fontSize));

	FontInstance instance = {};

	instance.image = gpuImage.GetPointer();

	const auto numBytes = gpuImage->GetNumberOfBytes();
	TByte* data = new TByte[numBytes];

	USize32 currentX = 0;
	for (USize32 c = 0; c < 255; c++) {
		for (USize32 i = 0; i < ftCharacters[c].sizeY; i++) {
			memcpy(&data[currentX + gpuImageSize.x * i],
				&ftCharacters[c].data.GetPointer()[ftCharacters[c].sizeX * i],
				ftCharacters[c].sizeX);
		}

		FontCharacter fontChar{};

		fontChar.size = { 
			static_cast<float>(ftCharacters[c].sizeX), 
			static_cast<float>(ftCharacters[c].sizeY) };

		fontChar.bearing = { 
			static_cast<float>(ftCharacters[c].left), 
			static_cast<float>(ftCharacters[c].top) };

		fontChar.advance = ftCharacters[c].advanceX;

		fontChar.texCoords = { 
			static_cast<int>(currentX), 
			0, 
			static_cast<int>(fontChar.size.x), 
			static_cast<int>(fontChar.size.y) };

		instance.characters[static_cast<char>(c)] = fontChar;

		currentX += ftCharacters[c].sizeX;
	}

	auto finalPixels = DynamicArray<TByte>::CreateReservedArray(numBytes);
	for (USize32 i = 0; i < numBytes / 4; i++) {
		finalPixels.Insert(255);
		finalPixels.Insert(255);
		finalPixels.Insert(255);
		finalPixels.Insert(data[i]);
	}

	// Creamos la lista de comandos para subir el recurso.
	// Preferir cola exclusiva de transferencia.
	OwnedPtr<ICommandList> copyCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);

	copyCmdList->Reset();
	copyCmdList->Start();

	copyCmdList->SetGpuImageBarrier(
		gpuImage.GetPointer(),
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	Engine::GetRenderer()->UploadImageToGpu(gpuImage.GetPointer(), finalPixels.GetData(), numBytes, copyCmdList.GetPointer());

	copyCmdList->SetGpuImageBarrier(
		gpuImage.GetPointer(), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ));

	// Transferimos la imagen a la cola principal.
	copyCmdList->TransferToQueue(
		gpuImage.GetPointer(),
		*Engine::GetRenderer()->GetMainRenderingQueue());

	copyCmdList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(copyCmdList.GetPointer());

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	instance.sprite = new Sprite;
	instance.sprite->SetImageView(gpuImage->GetView(viewConfig));

	m_instances[fontSize] = std::move(instance);

	delete[] data;
	// Memory free
	FT_Done_Face(face);
}

void Font::UnloadSizedFont(USize32 size) {
	m_instances.erase(size);
}

FontInstance& Font::GetInstance(USize32 fontSize) {
	if (m_instances.contains(fontSize))
		return m_instances.at(fontSize);

	LoadSizedFont(fontSize);

	return m_instances.at(fontSize);
}

const FontInstance& Font::GetExistingInstance(USize32 fontSize) const {
	OSK_ASSERT(m_instances.contains(fontSize), InvalidArgumentException(std::format("No existe la instancia con el tamaño {}.", fontSize)));
	return m_instances.at(fontSize);
}

bool Font::ContainsInstance(USize32 fontSize) const {
	return m_instances.contains(fontSize);
}

GpuImage* Font::GetGpuImage(USize32 fontSize) {
	return GetInstance(fontSize).image.GetPointer();
}

const FontCharacter& Font::GetCharacterInfo(USize32 fontSize, char character) {
	return GetInstance(fontSize).characters.at(character);
}

void Font::SetMaterial(Material* material) {
	m_material = material;
}

Material* Font::GetMaterial() const {
	return m_material;
}
