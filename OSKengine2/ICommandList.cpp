#include "ICommandList.h"

#include "IGpuImage.h"

using namespace OSK;

void ICommandList::TransitionImageLayout(GpuImage* image, GpuImageLayout next) {
	TransitionImageLayout(image, image->GetLayout(), next);
}
