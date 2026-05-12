#include "RgImageDependency.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

RgImageDependency RgImageDependency::ColorTarget(GdrImageUuid imageRefId) {
	RgImageDependency imageDependency{};

	imageDependency.imageRefId = imageRefId;
	imageDependency.range = GpuImageRange{};
	imageDependency.stage = GpuCommandStage::COLOR_ATTACHMENT_OUTPUT;
	imageDependency.type = RgDependencyType::WRITE;
	imageDependency.usage = GpuExclusiveImageUsage::COLOR;
	imageDependency.viewConfig = GpuImageViewConfig::CreateTarget_Color();

	return imageDependency;
}

RgImageDependency RgImageDependency::ComputeTarget(GdrImageUuid imageRefId, const std::string& slot, const std::string& binding) {
	RgImageDependency imageDependency{};

	imageDependency.imageRefId = imageRefId;
	imageDependency.range = GpuImageRange{};
	imageDependency.stage = GpuCommandStage::COMPUTE_SHADER;
	imageDependency.type = RgDependencyType::WRITE;
	imageDependency.usage = GpuExclusiveImageUsage::COMPUTE_OR_RT_TARGET;
	imageDependency.viewConfig = GpuImageViewConfig::CreateStorage_Default();
	imageDependency.shaderBinding.slotName = slot;
	imageDependency.shaderBinding.bindingName = binding;

	return imageDependency;
}

RgImageDependency RgImageDependency::Sampled_Fragment_SingleMipLevel(GdrImageUuid imageRefId, const std::string& slot, const std::string& binding) {
	RgImageDependency imageDependency{};

	imageDependency.imageRefId = imageRefId;
	imageDependency.range = GpuImageRange{};
	imageDependency.stage = GpuCommandStage::FRAGMENT_SHADER;
	imageDependency.type = RgDependencyType::READ;
	imageDependency.usage = GpuExclusiveImageUsage::SAMPLED;
	imageDependency.viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	imageDependency.shaderBinding.slotName = slot;
	imageDependency.shaderBinding.bindingName = binding;

	return imageDependency;
}
