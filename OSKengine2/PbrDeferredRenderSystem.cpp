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
	normalImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ Engine::GetWindow()->GetWindowSize().X, Engine::GetWindow()->GetWindowSize().Y, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D, 1, OSK::GRAPHICS::Format::RGBA8_UNORM,
		OSK::GRAPHICS::GpuImageUsage::COLOR | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
	colorImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ Engine::GetWindow()->GetWindowSize().X, Engine::GetWindow()->GetWindowSize().Y, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D, 1, OSK::GRAPHICS::Format::RGBA8_UNORM,
		OSK::GRAPHICS::GpuImageUsage::COLOR | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();

	normalRenderpass = Engine::GetRenderer()->CreateSecondaryRenderpass(normalImage.GetPointer()).GetPointer();
	colorRenderpass = Engine::GetRenderer()->CreateSecondaryRenderpass(colorImage.GetPointer()).GetPointer();

	IRenderSystem::CreateTargetImage(size);
}

void PbrDeferredRenderSystem::Resize(const Vector2ui& windowSize) {
	normalImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ windowSize.X, windowSize.Y, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D, 1, OSK::GRAPHICS::Format::RGBA8_UNORM,
		OSK::GRAPHICS::GpuImageUsage::COLOR | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
	colorImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ 1920, 1080, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D, 1, OSK::GRAPHICS::Format::RGBA8_UNORM,
		OSK::GRAPHICS::GpuImageUsage::COLOR | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();

	normalRenderpass->SetImages(normalImage.GetPointer(), normalImage.GetPointer(), normalImage.GetPointer());
	colorRenderpass->SetImages(colorImage.GetPointer(), colorImage.GetPointer(), colorImage.GetPointer());

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

	commandList->BeginRenderpass(normalRenderpass.GetPointer());

	OSK::Vector4ui imageRec = {
		0,
		0,
		normalImage->GetSize().X,
		normalImage->GetSize().Y
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

	commandList->EndRenderpass(normalRenderpass.GetPointer());
}

void PbrDeferredRenderSystem::ExecuteColorRenderpass(ICommandList* commandList) {
	Material* previousMaterial = nullptr;
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginRenderpass(colorRenderpass.GetPointer());

	OSK::Vector4ui imageRec = {
		0,
		0,
		colorImage->GetSize().X,
		colorImage->GetSize().Y
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

	commandList->EndRenderpass(colorRenderpass.GetPointer());
}
