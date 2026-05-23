#include "ISdfRenderPass.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


ISdfRenderPass::ISdfRenderPass() : IRenderPass(std::nullopt, RenderPassType::GRAPHICS) {
	m_sdfRenderer = MakeUnique<SdfBindlessRenderer2D>(
		Engine::GetEcs(),
		Engine::GetRenderer()->GetAllocator(),
		Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(SdfBindlessRenderer2D::DefaultMaterialPath));
}

ISdfRenderer2D* ISdfRenderPass::GetSdfRenderer() {
	return m_sdfRenderer.GetPointer();
}
