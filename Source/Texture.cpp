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
	return m_size;
}

USize32 Texture::GetNumberOfChannels() const {
	return m_numChannels;
}

const GRAPHICS::GpuImage* Texture::GetGpuImage() const {
	return m_image.GetPointer();
}

const IGpuImageView& Texture::GetTextureView2D() const {
	return *m_image->GetView(GpuImageViewConfig::CreateSampled_Default());
}

const IGpuImageView& Texture::GetTextureView() const {
	return *m_image->GetView(GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
}

void Texture::_SetSize(const Vector2ui size) {
	m_size = size;
}

void Texture::_SetNumberOfChannels(USize32 numChannels) {
	m_numChannels = numChannels;
}

void Texture::_SetImage(UniquePtr<GRAPHICS::GpuImage>&& image) {
	m_image = std::move(image);
}
