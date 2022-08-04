#include "IComputePipeline.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IPipelineLayout* IComputePipeline::GetLayout() const {
	return layout.GetPointer();
}
