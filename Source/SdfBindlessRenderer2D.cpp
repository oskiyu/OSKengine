#include "SdfBindlessRenderer2D.h"

#include "Transform2D.h"
#include "GameObject.h"
#include "GpuMemoryTypes.h"
#include "GpuQueueTypes.h"
#include "CameraComponent2D.h"
#include "OSKengine.h"
#include "ICommandList.h"
#include "Sprite.h"

#include "SdfDrawCall2D.h"
#include "SdfGlobalInformationBufferContent2D.h"
#include "SdfBindlessBufferContent2D.h"
#include <string>
#include <format>
#include "OwnedPtr.h"
#include "SdfBindlessBatch2D.h"
#include "GpuBufferRange.h"
#include "IMaterialSlot.h"
#include "MaterialInstance.h"
#include "SdfStringInfo.h"
#include "Font.h"


OSK::GRAPHICS::SdfBindlessRenderer2D::SdfBindlessRenderer2D(
	OSK::ECS::EntityComponentSystem* ecs, 
	IGpuMemoryAllocator* allocator, 
	Material* defaultMaterial) 
	: ISdfRenderer2D(ecs), m_ecs(ecs), m_memoryAllocator(allocator) 
{
	SetMaterial(defaultMaterial);

	for (UIndex32 i = 0; i < m_globalInformationInstances.size(); i++) {
		m_globalInformationBuffers[i] = allocator->CreateBuffer(
			sizeof(SdfGlobalInformationBufferContent2D),
			GPU_MEMORY_NO_ALIGNMENT,
			GpuBufferUsage::UNIFORM_BUFFER,
			GpuSharedMemoryType::GPU_AND_CPU,
			GpuQueueType::MAIN).GetPointer();

		m_globalInformationInstances[i] = defaultMaterial->CreateInstance().GetPointer();

		GetGlobalInformationSlot(i)->SetUniformBuffer("globalInformation", m_globalInformationBuffers[i].GetValue());
		GetGlobalInformationSlot(i)->SetDebugName("Global Information Slot");
		GetGlobalInformationSlot(i)->FlushUpdate();
	}
}


void OSK::GRAPHICS::SdfBindlessRenderer2D::SetMaterial(Material* material) {
	ISdfRenderer2D::SetMaterial(material);
	CreateNewBatch(material, Engine::GetRenderer()->GetCurrentFrameIndex());
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::Begin(ICommandList* commandList) {
	ISdfRenderer2D::Begin(commandList);

	m_currentBatchIndex = 0;
	m_insideBatchIndex = 0;
	m_batches.Empty();

	SdfGlobalInformationBufferContent2D bufferContent{};
	bufferContent.resolution = commandList->GetCurrentViewport().rectangle.GetRectangleSize().ToVector2f();
	bufferContent.cameraMatrix = GetCurrentCameraMatrix();

	GpuBuffer* targetBuffer = m_globalInformationBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()].GetPointer();

	targetBuffer->MapMemory();
	targetBuffer->ResetCursor();
	targetBuffer->Write(bufferContent);
	targetBuffer->Unmap();
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::Draw(const SdfDrawCall2D& drawCall) {
	const auto currentResourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	SdfBindlessBufferContent2D content = SdfBindlessBufferContent2D::From(drawCall);
	content.matrix = GetCurrentCameraMatrix() * content.matrix;

	if (m_batches.IsEmpty() || m_batches.Peek().drawCalls.GetSize() == MAX_DRAW_CALLS_PER_BATCH) {
		CreateNewBatch(GetCurrentMaterial(), currentResourceIndex);
	}

	auto& targetBatch = m_batches.Peek();
	targetBatch.drawCalls.Insert(drawCall);

	auto* buffer = m_drawCallsBuffers[currentResourceIndex][m_currentBatchIndex].GetPointer();

	GpuBufferRange bufferRange{};
	bufferRange.full = false;
	bufferRange.offset = buffer->GetCursor();
	bufferRange.size = sizeof(content) + GetDrawCallBufferContentPadding();

	buffer->Write(content);

	// TODO: reemplazar con mover el cursor.
	for (UIndex32 i = 0; i < GetDrawCallBufferContentPadding(); i++) {
		buffer->Write<TByte>(0);
	}

	const IGpuImageView* view = drawCall.contentType == SdfDrawCallContentType2D::TEXTURE
		? drawCall.texture
		: nullptr;

	const IGpuImageSampler* sampler = drawCall.sampler ? drawCall.sampler : &Engine::GetRenderer()->GetSampler(drawCall.samplerDesc);
	EnsureMaterialSlotContent(*buffer, view, sampler, bufferRange, currentResourceIndex);

	m_insideBatchIndex++;
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::Draw(const OSK::GRAPHICS::SdfStringInfo& stringCall) {
	const auto drawCalls = GetTextDrawCalls(stringCall);

	for (const auto& drawCall : drawCalls) {
		Draw(drawCall);
	}
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::End() {
	if (m_batches.IsEmpty()) {
		return;
	}

	const auto currentResourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	// Flushes
	for (auto& mInstance : m_drawCallsBuffersInstances[currentResourceIndex]) {
		mInstance->GetSlot(DrawCallsSlotName)->FlushUpdate();
	}

	// Renderizado final
	GetCurrentCommandList()->BindVertexBuffer(*Sprite::globalVertexBuffer);
	GetCurrentCommandList()->BindIndexBuffer(*Sprite::globalIndexBuffer);

	GetCurrentCommandList()->BindMaterial(*m_batches[0].material);
	GetCurrentCommandList()->BindMaterialSlot(*GetGlobalInformationSlot(currentResourceIndex));

	const Material* previousMaterial = m_batches[0].material;

	for (UIndex64 i = 0; i < m_batches.GetSize(); i++) {
		const SdfBindlessBatch2D& batch = m_batches[i];

		if (batch.material != previousMaterial) {
			GetCurrentCommandList()->BindMaterial(*batch.material);
			previousMaterial = batch.material;
			GetCurrentCommandList()->BindMaterialSlot(*GetGlobalInformationSlot(currentResourceIndex));
		}

		const IMaterialSlot* instanceSlot = m_drawCallsBuffersInstances[currentResourceIndex][i]->GetSlot(DrawCallsSlotName);

		GetCurrentCommandList()->BindMaterialSlot(*instanceSlot);

		GetCurrentCommandList()->DrawInstances(0, 6, 0, static_cast<USize32>(batch.drawCalls.GetSize()));
	}

	for (auto& buffer : m_drawCallsBuffers[currentResourceIndex]) {
		buffer->ResetCursor();
	}

	ISdfRenderer2D::End();
}

OSK::GRAPHICS::IMaterialSlot* OSK::GRAPHICS::SdfBindlessRenderer2D::GetGlobalInformationSlot(UIndex32 resourceIndex) {
	return m_globalInformationInstances[resourceIndex]->GetSlot(GlobalMaterialSlotName);
}

OSK::USize64 OSK::GRAPHICS::SdfBindlessRenderer2D::GetDrawCallBufferContentPadding() const {
	const USize64 necessaryAlignment = m_memoryAllocator->GetAlignment(1, GpuBufferUsage::UNIFORM_BUFFER);
	const USize64 necessaryPadding = necessaryAlignment - (sizeof(SdfBindlessBufferContent2D) % necessaryAlignment);

	return necessaryPadding;
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::CreateNewBatch(const Material* material, UIndex32 resourceIndex) {
	if (m_batches.GetSize() >= m_drawCallsBuffers[resourceIndex].GetSize()) {
		// Añadimos un nuevo buffer y material slot (si no existen).
		AddContentBuffer(resourceIndex);
	}

	// Aumentamos el índice de batch, si ya había alguno.
	if (!m_batches.IsEmpty()) {
		m_currentBatchIndex++;
	}
	m_insideBatchIndex = 0;

	m_batches.Insert({});
	m_batches.Peek().material = material;
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::AddContentBuffer(UIndex32 resourceIndex) {
	const USize64 necessaryPadding = GetDrawCallBufferContentPadding();
	const USize64 elementSizeWithPadding = sizeof(SdfBindlessBufferContent2D) + necessaryPadding;

	// Buffer.
	OwnedPtr<GpuBuffer> buffer = m_memoryAllocator->CreateBuffer(
		elementSizeWithPadding * MAX_DRAW_CALLS_PER_BATCH,
		GPU_MEMORY_NO_ALIGNMENT,
		GpuBufferUsage::UNIFORM_BUFFER,
		GpuSharedMemoryType::GPU_AND_CPU,
		GpuQueueType::MAIN);

	buffer->MapMemory();

	m_drawCallsBuffers[resourceIndex].Insert(buffer.GetPointer());

	// Material.
	const std::string materialInstanceName = std::format("Instances Slot {} Frame {}", m_drawCallsBuffersInstances[resourceIndex].GetSize(), resourceIndex);

	OwnedPtr<MaterialInstance> materialInstance = GetCurrentMaterial()->CreateInstance();
	materialInstance->GetSlot(DrawCallsSlotName)->SetDebugName(materialInstanceName);
	m_drawCallsBuffersInstances[resourceIndex].Insert(materialInstance.GetPointer());
}

void OSK::GRAPHICS::SdfBindlessRenderer2D::EnsureMaterialSlotContent(
	const GpuBuffer& buffer, 
	const IGpuImageView* imageView,
	const IGpuImageSampler* sampler, 
	const GpuBufferRange& range, 
	USize32 resourceIndex) 
{
	MaterialInstance* instance = m_drawCallsBuffersInstances[resourceIndex][m_currentBatchIndex].GetPointer();
	IMaterialSlot* slot = instance->GetSlot(DrawCallsSlotName);

	slot->SetUniformBuffer(DrawCallsBindingName, buffer, range, m_insideBatchIndex);

	if (imageView) {
		slot->SetGpuImage(DrawCallsImageBindingName, *imageView, *sampler, m_insideBatchIndex);
	}

	slot->FlushUpdate();
}
