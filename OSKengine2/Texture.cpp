#include "Texture.h"

#include "OSKengine.h"
#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::ASSETS;

Texture::Texture(const std::string& assetFile)
	: IAsset(assetFile) {

}

Texture::~Texture() {
	delete image.GetPointer();
}

Vector2ui Texture::GetSize() const {
	return size;
}

TSize Texture::GetNumberOfChannels() const {
	return numChannels;
}

GRAPHICS::GpuImage* Texture::GetGpuImage() const {
	return image.GetPointer();
}

void Texture::_SetSize(const Vector2ui size) {
	this->size = size;
}

void Texture::_SetNumberOfChannels(TSize numChannels) {
	this->numChannels = numChannels;
}

void Texture::_SetImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	this->image = image;
}
