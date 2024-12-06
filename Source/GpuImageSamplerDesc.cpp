#include "GpuImageSamplerDesc.h"

#include "IGpuImage.h"


OSK::GRAPHICS::GpuImageSamplerDesc OSK::GRAPHICS::GpuImageSamplerDesc::CreateDefault_NoMipMap() {
	return { .mipMapMode = GpuImageMipmapMode::NONE };
}

OSK::GRAPHICS::GpuImageSamplerDesc OSK::GRAPHICS::GpuImageSamplerDesc::CreateDefault_WithMipMap(const GpuImage& image) {
	return { 
		.mipMapMode = GpuImageMipmapMode::AUTO,
		.maxMipLevel = GpuImage::GetMipLevels(
			image.GetSize2D().x,
			image.GetSize2D().y )};
}
