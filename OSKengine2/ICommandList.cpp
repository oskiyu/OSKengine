#include "ICommandList.h"

#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandList::~ICommandList() {
	DeleteAllStagingBuffers();
}

void ICommandList::TransitionImageLayout(GpuImage* image, GpuImageLayout next) {
	TransitionImageLayout(image, image->GetLayout(), next);
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
