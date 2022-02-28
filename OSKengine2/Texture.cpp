#include "Texture.h"

using namespace OSK;

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

GpuImage* Texture::GetGpuImage() const {
	return image.GetPointer();
}

void Texture::_SetSize(const Vector2ui size) {
	this->size = size;
}

void Texture::_SetNumberOfChannels(TSize numChannels) {
	this->numChannels = numChannels;
}

void Texture::_SetImage(OwnedPtr<GpuImage> image) {
	this->image = image;
}
