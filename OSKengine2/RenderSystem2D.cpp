#include "RenderSystem2D.h"

#include "Sprite.h"
#include "Transform2D.h"
#include "EntityComponentSystem.h"
#include "OSKengine.h"
#include "ICommandList.h"
#include "MaterialLayout.h"
#include "PushConst2D.h"
#include "IGpuImage.h"

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

	spriteRenderer.Begin();
	PushConst2D pushConst{};

	for (GameObjectIndex obj : GetObjects()) {
		spriteRenderer.Draw(
			Engine::GetEntityComponentSystem()->GetComponent<Sprite>(obj), 
			Engine::GetEntityComponentSystem()->GetComponent<Transform2D>(obj)
		);
	}

	spriteRenderer.End();
}
