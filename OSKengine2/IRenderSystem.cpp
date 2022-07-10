#include "IRenderSystem.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "Format.h"
#include "Material.h"
#include "Viewport.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void IRenderSystem::OnCreate() {
	// Si el renderizador está activo, se crean las imágenes.
	// Si no, se crearán cuando el renderizador se inicialice.
	if (Engine::GetRenderer()->IsOpen())
		CreateTargetImage(Engine::GetWindow()->GetWindowSize());

	ISystem::OnCreate();
}

void IRenderSystem::Resize(const Vector2ui& windowSize) {
	renderTarget.Resize(windowSize);
}

void IRenderSystem::CreateTargetImage(const Vector2ui& size) {
	renderTarget.Create(size, Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);
}

RenderTarget& IRenderSystem::GetRenderTarget() {
	return renderTarget;
}

void IRenderSystem::SetupViewport(GRAPHICS::ICommandList* commandList) {
	Vector4ui windowRec = {
		0,
		0,
		renderTarget.GetSize().X,
		renderTarget.GetSize().Y
	};

	Viewport viewport{};
	viewport.rectangle = windowRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(windowRec);
}
