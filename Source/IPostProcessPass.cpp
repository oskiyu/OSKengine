#include "IPostProcessPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "MaterialSystem.h"
#include "IGpuImageView.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void IPostProcessPass::Create(const Vector2ui& size) {
	RenderTargetAttachmentInfo info{};
	info.format = Format::RGBA16_SFLOAT;
	info.usage = GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_SOURCE;
	info.sampler = GpuImageSamplerDesc::CreateDefault_NoMipMap();
	m_resolveRenderTarget.Create(size, info);
}

void IPostProcessPass::Resize(const Vector2ui& size) {
	m_resolveRenderTarget.Resize(size);
}

ComputeRenderTarget& IPostProcessPass::GetOutput() {
	return m_resolveRenderTarget;
}

const ComputeRenderTarget& IPostProcessPass::GetOutput() const {
	return m_resolveRenderTarget;
}
