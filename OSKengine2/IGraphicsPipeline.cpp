#include "IGraphicsPipeline.h"

using namespace OSK;

IPipelineLayout* IGraphicsPipeline::GetLayout() const {
	return layout.GetPointer();
}
