#include "IComputePipeline.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

const IPipelineLayout* IComputePipeline::GetLayout() const {
	return layout.GetPointer();
}
