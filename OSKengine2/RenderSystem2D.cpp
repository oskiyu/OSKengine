#include "RenderSystem2D.h"

#include "SpriteComponent.h"
#include "Transform2D.h"
#include "EntityComponentSystem.h"
#include "OSKengine.h"
#include "ICommandList.h"
#include "MaterialLayout.h"
#include "PushConst2D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

RenderSystem2D::RenderSystem2D() {
	Signature signature{};

	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform2D>());
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<SpriteComponent>());

	SetSignature(signature);
}

void RenderSystem2D::Render(GRAPHICS::ICommandList* commandList) {
	commandList->BindVertexBuffer(Sprite::globalVertexBuffer);
	commandList->BindIndexBuffer(Sprite::globalIndexBuffer);

	PushConst2D pushConst{};

	for (GameObjectIndex obj : GetObjects()) {
		const SpriteComponent& sprite = Engine::GetEntityComponentSystem()->GetComponent<SpriteComponent>(obj);
		const Transform2D& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform2D>(obj);

		pushConst.color = sprite.GetSprite().color;
		pushConst.matrix = transform.GetAsMatrix();
		pushConst.texCoords = sprite.GetSprite().GetTexCoords();

		commandList->BindMaterial(sprite.GetMaterialInstance()->GetMaterial());

		commandList->PushMaterialConstants("sprite", pushConst);

		for (const std::string& slotName : sprite.GetMaterialInstance()->GetLayout()->GetAllSlotNames())
			commandList->BindMaterialSlot(sprite.GetMaterialInstance()->GetSlot(slotName));

		commandList->DrawSingleInstance(6);
	}
}
