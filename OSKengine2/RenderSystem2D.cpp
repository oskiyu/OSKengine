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

	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform2D>());
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Sprite>());

	SetSignature(signature);
}

void RenderSystem2D::Render(ICommandList* commandList) {
	spriteRenderer.SetCommandList(commandList);

	commandList->BeginAndClearRenderpass(renderTarget.GetTargetRenderpass(), { 1.0f, 1.0f, 1.0f, 0.0f });
	
	Vector4ui windowRec = {
		0,
		0,
		Engine::GetWindow()->GetWindowSize().X,
		Engine::GetWindow()->GetWindowSize().Y
	};

	Viewport viewport{};
	viewport.rectangle = windowRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(windowRec);

	spriteRenderer.Begin();

	for (GameObjectIndex obj : GetObjects()) {
		spriteRenderer.Draw(
			Engine::GetEntityComponentSystem()->GetComponent<Sprite>(obj), 
			Engine::GetEntityComponentSystem()->GetComponent<Transform2D>(obj)
		);
	}

	spriteRenderer.End();

	commandList->EndRenderpass(renderTarget.GetTargetRenderpass());
}
