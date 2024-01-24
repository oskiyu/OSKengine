#include "IGraphicsPipeline.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

const IPipelineLayout* IGraphicsPipeline::GetLayout() const {
	return layout.GetPointer();
}
