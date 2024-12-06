#include "IMeshPipeline.h"

const OSK::GRAPHICS::IPipelineLayout* OSK::GRAPHICS::IMeshPipeline::GetLayout() const {
	return m_layout.GetPointer();
}
