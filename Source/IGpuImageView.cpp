#include "IGpuImageView.h"

#include "Assert.h"
#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuImageView::IGpuImageView(
	const GpuImage& originalImage,
	const GpuImageViewConfig& config)

	: config(config), originalImage(&originalImage) {
	size = originalImage.GetSize3D();
}
