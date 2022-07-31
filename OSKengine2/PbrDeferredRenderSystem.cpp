#include "PbrDeferredRenderSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "ICommandList.h"
#include "IRenderer.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"
#include "Format.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Viewport.h"
#include "Window.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

PbrDeferredRenderSystem::PbrDeferredRenderSystem() {
	Signature signature{};

	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<ModelComponent3D>());

	SetSignature(signature);
}

void PbrDeferredRenderSystem::CreateTargetImage(const Vector2ui& size) {
	normalRenderTarget.Create(size, Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);
	colorRenderTarget.Create(size, Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);

	IRenderSystem::CreateTargetImage(size);
}

void PbrDeferredRenderSystem::Resize(const Vector2ui& windowSize) {
	normalRenderTarget.Resize(windowSize);
	colorRenderTarget.Resize(windowSize);

	IRenderSystem::Resize(windowSize);
}

void PbrDeferredRenderSystem::Render(ICommandList* commandList) {
	ExecuteNormalRenderpass(commandList);
	ExecuteColorRenderpass(commandList);
}

void PbrDeferredRenderSystem::ExecuteNormalRenderpass(ICommandList* commandList) {
	Material* previousMaterial = nullptr;
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginGraphicsRenderpass(&normalRenderTarget);

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

	commandList->EndGraphicsRenderpass();
}

void PbrDeferredRenderSystem::ExecuteColorRenderpass(ICommandList* commandList) {
	Material* previousMaterial = nullptr;
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginGraphicsRenderpass(&normalRenderTarget);

	OSK::Vector4ui imageRec = {
		0,
		0,
		normalRenderTarget.GetSize().X,
		normalRenderTarget.GetSize().Y
	};

	OSK::GRAPHICS::Viewport viewport{};
	viewport.rectangle = imageRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(imageRec);

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

	commandList->EndGraphicsRenderpass();
}
