#include "RenderSystem3D.h"

#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "ICommandList.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Viewport.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

RenderSystem3D::RenderSystem3D() {
	Signature signature{};

	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<ModelComponent3D>());

	SetSignature(signature);
}

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList) {
	Material* previousMaterial = nullptr;
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginAndClearRenderpass(&renderTarget, { 0.0f, 0.0f, 0.0f, 0.0f });

	SetupViewport(commandList);

	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEntityComponentSystem()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj);

		if (previousMaterial != model.GetMaterial()) {
			commandList->BindMaterial(model.GetMaterial());
			previousMaterial = model.GetMaterial();
		}

		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}
		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		commandList->PushMaterialConstants("model", transform.GetAsMatrix());

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			for (const std::string& slotName : model.GetMaterial()->GetLayout()->GetAllSlotNames())
				commandList->BindMaterialSlot(model.GetMeshMaterialInstance(i)->GetSlot(slotName));

			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
		}
	}

	commandList->EndRenderpass(&renderTarget);
}
