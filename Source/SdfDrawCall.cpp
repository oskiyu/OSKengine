#include "SdfDrawCall2D.h"

#include "GpuImageViewConfig.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

SdfDrawCall2D SdfDrawCall2D::Texture(const IGpuImageView& view) {
	SdfDrawCall2D drawCall{};

	drawCall.contentType = SdfDrawCallContentType2D::TEXTURE;
	drawCall.shape = SdfShape2D::RECTANGLE;
	drawCall.contentType = SdfDrawCallContentType2D::TEXTURE;
	drawCall.mainColor = Color::White;
	auto viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	viewConfig.channel = SampledChannel::COLOR;
	drawCall.texture = &view;

	return drawCall;
}