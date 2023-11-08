#include "RenderSystem2D.h"

#include "Sprite.h"
#include "Transform2D.h"
#include "EntityComponentSystem.h"
#include "OSKengine.h"
#include "ICommandList.h"
#include "MaterialLayout.h"
#include "PushConst2D.h"
#include "IGpuImage.h"
#include "Viewport.h"
#include "Color.hpp"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

RenderSystem2D::RenderSystem2D() {
	Signature signature{};

	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform2D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Sprite>());

	_SetSignature(signature);
}

void RenderSystem2D::Render(ICommandList* commandList) {
	spriteRenderer.SetCommandList(commandList);

	commandList->StartDebugSection("2D Rendering", Color::Blue);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, { 1.0f, 1.0f, 1.0f, 0.0f });

	SetupViewport(commandList);

	spriteRenderer.Begin();

	for (GameObjectIndex obj : GetObjects()) {
		spriteRenderer.Draw(
			Engine::GetEcs()->GetComponent<Sprite>(obj),
			Engine::GetEcs()->GetComponent<Transform2D>(obj)
		);
	}

	spriteRenderer.End();

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}
