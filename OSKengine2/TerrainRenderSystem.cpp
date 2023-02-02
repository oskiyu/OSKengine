#include "TerrainRenderSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "TerrainComponent.h"
#include "ICommandList.h"
#include "Material.h"
#include "MaterialInstance.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

TerrainRenderSystem::TerrainRenderSystem() {
	Signature signature{};

	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<TerrainComponent>());

	_SetSignature(signature);
}

void TerrainRenderSystem::Render(ICommandList* commandList) {
	for (const GameObjectIndex obj : GetObjects()) {
		const TerrainComponent& model = Engine::GetEcs()->GetComponent<TerrainComponent>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		commandList->BindMaterial(model.GetMaterialInstance()->GetMaterial());

		commandList->BindVertexBuffer(model.GetVertexBuffer());
		commandList->BindIndexBuffer(model.GetIndexBuffer());
		commandList->PushMaterialConstants("model", transform.GetAsMatrix());

		for (const auto& [slotName, slot] : model.GetMaterialInstance()->GetLayout()->GetAllSlots())
			commandList->BindMaterialSlot(model.GetMaterialInstance()->GetSlot(slotName));

		commandList->DrawSingleMesh(0, model.GetNumIndices());
	}
}
