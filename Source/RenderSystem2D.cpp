#include "RenderSystem2D.h"

#include "Sprite.h"
#include "TransformComponent2D.h"
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

RenderSystem2D::RenderSystem2D() : spriteRenderer(Engine::GetRenderer()->GetAllocator(), Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_2d.json")) {
	Signature signature{};

	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform2D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Sprite>());

	_SetSignature(signature);
}

void RenderSystem2D::Render(ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
	spriteRenderer.SetCommandList(commandList);

	commandList->StartDebugSection("2D Rendering", Color::Blue);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, { 1.0f, 1.0f, 1.0f, 0.0f });

	SetupViewport(commandList);

	spriteRenderer.Begin();

	for (const GameObjectIndex obj : objects) {
		spriteRenderer.Draw(
			Engine::GetEcs()->GetComponent<Sprite>(obj),
			Engine::GetEcs()->GetComponent<Transform2D>(obj)
		);
	}

	spriteRenderer.End();

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

nlohmann::json RenderSystem2D::SaveConfiguration() const {
	auto output = nlohmann::json();


	return output;
}

PERSISTENCE::BinaryBlock RenderSystem2D::SaveBinaryConfiguration() const {
	auto output = PERSISTENCE::BinaryBlock::Empty();

	return output;
}

void RenderSystem2D::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {

}

void RenderSystem2D::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	
}
