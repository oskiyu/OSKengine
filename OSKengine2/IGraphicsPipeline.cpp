#include "IGraphicsPipeline.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IPipelineLayout* IGraphicsPipeline::GetLayout() const {
	return layout.GetPointer();
}
