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

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Font::Font(const std::string& assetFile) : IAsset(assetFile) {

}

void Font::_SetFontFilePath(const std::string& rawFile) {
	this->fontFile = rawFile;
}

void Font::LoadSizedFont(TSize fontSize) {
	// Carga de la librer�a.
	static FT_Library freeType = nullptr;
	if (freeType == nullptr)
		OSK_ASSERT_FALSE(FT_Init_FreeType(&freeType), "No se ha podido cargar FreeType.");

	// Un FT_Face describe la tipograf�a de la fuente.
	FT_Face face = nullptr;

	// Carga de la fuente.
	FT_Error result = FT_New_Face(freeType, fontFile.c_str(), 0, &face);
	OSK_ASSERT_FALSE(result, "No se ha podido cargar la fuente. C�digo: " + std::to_string(result));

	// Establece el tama�o de esta instancia en concreto.
	FT_Set_Pixel_Sizes(face, 0, fontSize);


	// Definir� cada uno de los caracteres.
	struct FtChar {
		UniquePtr<TByte> data;
		TSize sizeX = 0;
		TSize sizeY = 0;
		TSize left = 0;
		TSize top = 0;
		TSize advanceX = 0;
	};

	FtChar ftCharacters[255];

	Vector2ui gpuImageSize = { 0, 0 };

	// Cargamos todos los caracteres.
	// Actualizamos el tama�o de la imagen.
	for (TSize i = 0; i < 255; i++) {
		result = FT_Load_Char(face, i, FT_LOAD_RENDER);
		OSK_ASSERT_FALSE(result, "No se pudo cargar el caracter " + std::to_string((char)i) + ". C�digo: " + std::to_string(result));

		ftCharacters[i].sizeX = face->glyph->bitmap.width;
		ftCharacters[i].sizeY = face->glyph->bitmap.rows;

		ftCharacters[i].left = face->glyph->bitmap_left;
		ftCharacters[i].top = face->glyph->bitmap_top;

		ftCharacters[i].advanceX = face->glyph->advance.x;

		ftCharacters[i].data = new TByte[ftCharacters[i].sizeX * ftCharacters[i].sizeY];

		memcpy(ftCharacters[i].data.GetPointer(), face->glyph->bitmap.buffer, ftCharacters[i].sizeX* ftCharacters[i].sizeY);

		// Tama�o de la imagen.
		gpuImageSize.X += ftCharacters[i].sizeX;
		if (ftCharacters[i].sizeY > gpuImageSize.Y)
			gpuImageSize.Y = ftCharacters[i].sizeY;
	}

	// Creaci�n de la imagen.
	OwnedPtr<GpuImage> gpuImage = Engine::GetRenderer()->GetMemoryAllocator()
		->CreateImage({ gpuImageSize.X, gpuImageSize.Y, 1 }, GpuImageDimension::d2D, 1,
			Format::RGBA8_UNORM, GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_DESTINATION,
			GpuSharedMemoryType::GPU_ONLY, 1);


	instances.InsertMove(fontSize, std::move(FontInstance{}));
	FontInstance& instance = instances.Get(fontSize);

	instance.image = gpuImage.GetPointer();

	const TSize numBytes = gpuImage->GetNumberOfBytes();
	UniquePtr<TByte[]> data(new TByte[numBytes]);

	TSize currentX = 0;
	for (TSize c = 0; c < 255; c++) {
		for (TSize i = 0; i < ftCharacters[c].sizeY; i++) {
			memcpy(&data[currentX + gpuImageSize.X * i],
				&ftCharacters[c].data.GetPointer()[ftCharacters[c].sizeX * i],
				ftCharacters[c].sizeX);
		}

		FontCharacter fontChar{};
		fontChar.size = { (float)ftCharacters[c].sizeX, (float)ftCharacters[c].sizeY };
		fontChar.bearing = { (float)ftCharacters[c].left, (float)ftCharacters[c].top };
		fontChar.advance = ftCharacters[c].advanceX;
		fontChar.texCoords = { (int)currentX, 0, (int)fontChar.size.X, (int)fontChar.size.Y };

		instance.characters.Insert((char)c, fontChar);

		currentX += ftCharacters[c].sizeX;
	}

	auto finalPixels = DynamicArray<TByte>::CreateReservedArray(numBytes);
	for (TSize i = 0; i < numBytes / 4; i++) {
		finalPixels.Insert(255);
		finalPixels.Insert(255);
		finalPixels.Insert(255);
		finalPixels.Insert(data[i]);
	}

	Engine::GetRenderer()->UploadImageToGpu(gpuImage.GetPointer(), finalPixels.GetData(), numBytes, GpuImageLayout::SHADER_READ_ONLY);

	if (material) {
		instance.sprite = new Sprite;
		instance.sprite->SetMaterialInstance(material->CreateInstance());
		instance.sprite->SetGpuImage(gpuImage.GetPointer());
	}

	// Memory free
	FT_Done_Face(face);
}

void Font::UnloadSizedFont(TSize size) {
	instances.Remove(size);
}

const FontInstance& Font::GetInstance(TSize fontSize) {
	const auto& it = instances.Find(fontSize);
	if (!it.IsEmpty())
		return it.GetValue().second;

	LoadSizedFont(fontSize);

	return instances.Get(fontSize);
}

GpuImage* Font::GetGpuImage(TSize fontSize) {

	return GetInstance(fontSize).image.GetPointer();
}

const FontCharacter& Font::GetCharacterInfo(TSize fontSize, char character) {
	return GetInstance(fontSize).characters.Get(character);
}

void Font::SetMaterial(Material* material) {
	this->material = material;
}

Material* Font::GetMaterial() const {
	return material;
}