#include "IRenderpass.h"

#include "Assert.h"
#include "RenderpassType.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IRenderpass::IRenderpass(RenderpassType type) 
	: type(type) {

}

RenderpassType IRenderpass::GetType() const {
	return type;
}

GpuImage* IRenderpass::GetImage(TSize index) const {
	OSK_ASSERT(index >= 0 && index < GetNumberOfImages(), "Se ha intentado acceder a la imagen "
		+ std::to_string(index) + ", pero solo hay " + std::to_string(GetNumberOfImages()) + " imágenes.");

	return images[index];
}

TSize IRenderpass::GetNumberOfImages() const {
	if (images[0] == nullptr)
		return 0;

	if (images[1] == nullptr)
		return 1;

	return 3;
}
