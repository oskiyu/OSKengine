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
	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEntityComponentSystem()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj);

		commandList->BindMaterial(model.GetMaterialInstance()->GetMaterial());
		for (const std::string& slotName : model.GetMaterialInstance()->GetLayout()->GetAllSlotNames())
			commandList->BindMaterialSlot(model.GetMaterialInstance()->GetSlot(slotName));

		commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
		commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());
		commandList->PushMaterialConstants("model", transform.GetAsMatrix());

		for (const auto& mesh : model.GetModel()->GetMeshes())
			commandList->DrawSingleMesh(mesh.GetFirstIndexId(), mesh.GetNumberOfIndices());
	}
}
