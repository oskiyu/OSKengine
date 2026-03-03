#include "ExampleComputePass.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ExampleComputePass::ExampleComputePass(
	GdrBufferUuid bufferRef,
	GdrImageUuid imageRef) 
: 
	IRenderPass(
		"Resources/Materials/Testing/simple.json", 
		RenderPassType::COMPUTE) 
{
	RgBufferDependency bufferDependency{};
	bufferDependency.bufferRefId = bufferRef;
	bufferDependency.range.size = sizeof(int);
	bufferDependency.stage = GpuCommandStage::COMPUTE_SHADER;
	bufferDependency.type = RgDependencyType::READ;
	bufferDependency.usage = RgBufferUsage::UNIFORM_BUFFER;
	bufferDependency.shaderBinding.slotName = "buffer";
	bufferDependency.shaderBinding.bindingName = "bufferContent";

	AddBufferDependency(bufferDependency);

	RgImageDependency imageDependency{};
	imageDependency.imageRefId = imageRef;
	imageDependency.range = GpuImageRange{};
	imageDependency.stage = GpuCommandStage::COMPUTE_SHADER;
	imageDependency.type = RgDependencyType::WRITE;
	imageDependency.usage = GpuExclusiveImageUsage::COMPUTE_OR_RT_TARGET;
	imageDependency.shaderBinding.slotName = "image";
	imageDependency.shaderBinding.bindingName = "finalImage";
	imageDependency.viewConfig = GpuImageViewConfig::CreateStorage_Default();

	AddImageDependency(imageDependency);
}

void ExampleComputePass::Execute(ICommandList* cmdList) {
	cmdList->DispatchCompute({ 256, 256, 1 });
}
