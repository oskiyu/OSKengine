#include "IRenderSystem.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "Format.h"
#include "Material.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void IRenderSystem::Resize(const Vector2ui& windowSize) {
	renderTarget.Resize(windowSize);
}

void IRenderSystem::CreateTargetImage(const Vector2ui& size) {
	renderTarget.Create(size);
}

const RenderTarget& IRenderSystem::GetRenderTarget() const {
	return renderTarget;
}
