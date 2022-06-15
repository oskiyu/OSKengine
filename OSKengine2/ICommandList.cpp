#include "ICommandList.h"

#include "IGpuImage.h"
#include "OSKengine.h"
#include "IRenderer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandList::~ICommandList() {
	DeleteAllStagingBuffers();
}

void ICommandList::TransitionImageLayout(GpuImage* image, GpuImageLayout next, TSize baseLayer, TSize layerCount) {
	TransitionImageLayout(image, image->GetLayout(), next, baseLayer, layerCount);
}

void ICommandList::RegisterStagingBuffer(OwnedPtr<GpuDataBuffer> stagingBuffer) {
	stagingBuffersToDelete.Insert(stagingBuffer);
}

void ICommandList::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size) {
	PushMaterialConstants(pushConstName, data, size, 0);
}

void ICommandList::DeleteAllStagingBuffers() {
	for (TSize i = 0; i < stagingBuffersToDelete.GetSize(); i++)
		stagingBuffersToDelete.At(i).Delete();

	stagingBuffersToDelete.Free();
}

void ICommandList::_SetSingleTimeUse() {
	isSingleUse = true;
}

TSize ICommandList::GetCommandListIndex() const {
	return isSingleUse ? 0 : Engine::GetRenderer()->GetCurrentCommandListIndex();
}
