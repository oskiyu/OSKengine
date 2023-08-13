#include "IRaytracingPipeline.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

const IPipelineLayout* IRaytracingPipeline::GetLayout() const {
	return layout.GetPointer();
}

const IRtShaderTable* IRaytracingPipeline::GetShaderTable() const {
	return shaderTable.GetPointer();
}
