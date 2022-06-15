#include "IRaytracingPipeline.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IPipelineLayout* IRaytracingPipeline::GetLayout() const {
	return layout.GetPointer();
}

IRtShaderTable* IRaytracingPipeline::GetShaderTable() const {
	return shaderTable.GetPointer();
}
