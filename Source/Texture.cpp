#include "Texture.h"

#include "OSKengine.h"
#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Texture::Texture(const std::string& assetFile)
	: IAsset(assetFile) {

}

Vector2ui Texture::GetSize() const {
	return size;
}

USize32 Texture::GetNumberOfChannels() const {
	return numChannels;
}

GRAPHICS::GpuImage* Texture::GetGpuImage() const {
	return image.GetPointer();
}

const IGpuImageView& Texture::GetTextureView2D() const {
	return *image->GetView(GpuImageViewConfig::CreateSampled_Default());
}

const IGpuImageView& Texture::GetTextureView() const {
	return *image->GetView(GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
}

void Texture::_SetSize(const Vector2ui size) {
	this->size = size;
}

void Texture::_SetNumberOfChannels(USize32 numChannels) {
	this->numChannels = numChannels;
}

void Texture::_SetImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	this->image = image.GetPointer();
}
