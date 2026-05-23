#include "RenderToSwapchainPass.h"

#include "Platforms.h"
#include "RenderGraph.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


RenderToSwapchainPass::RenderToSwapchainPass(GdrImageUuid src, const std::span<GdrImageUuid, 3> targetImages) : ISdfRenderPass(), m_src(src) {
	AddImageDependency(RgImageDependency::Sampled_Fragment_SingleMipLevel(src, "", ""));

	for (UIndex16 i = 0; i < targetImages.size(); i++) {
		auto imageDependency = RgImageDependency::ColorTarget(targetImages[i]);
		imageDependency.clearIfTargetImage = false;
		imageDependency.frameInFlight = i;

		AddImageDependency(imageDependency);
	}
}

void RenderToSwapchainPass::Execute(ICommandList* cmdList) {
	auto* sdfRenderer = GetSdfRenderer();
	const auto& img = GetRenderGraph()->GetImage(m_src);

	sdfRenderer->Begin(cmdList);
	sdfRenderer->SetCamera(Vector2f::Zero, Vector2f::One);

	auto drawCall = SdfDrawCall2D::Texture(*img.GetView(GpuImageViewConfig::CreateSampled_SingleMipLevel(0)));
	drawCall.transform.SetPosition(Vector2f::Zero);
	drawCall.transform.SetScale(Vector2f::One);

	drawCall.samplerDesc = GpuImageSamplerDesc::CreateDefault_NoMipMap();
	drawCall.samplerDesc.filteringType = GpuImageFilteringType::NEAREST;

#ifdef OSK_ANDROID
	// Se gira la imagen 90º para mostrarse en horizontal.
	// @todo Implementar shader de computación para realizar el giro.
	drawCall.transform.SetRotation(90.0f);
#endif

	sdfRenderer->Draw(drawCall);

	sdfRenderer->End();
}

std::string_view RenderToSwapchainPass::GetName() const {
	return "RenderToSwapchainPass";
}
