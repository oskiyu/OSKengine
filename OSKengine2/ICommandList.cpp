#include "ICommandList.h"

#include "IGpuImage.h"

using namespace OSK;

ICommandList::~ICommandList() {
	DeleteAllStagingBuffers();
}

void ICommandList::TransitionImageLayout(GpuImage* image, GpuImageLayout next) {
	TransitionImageLayout(image, image->GetLayout(), next);
}

void ICommandList::RegisterStagingBuffer(OwnedPtr<GpuDataBuffer> stagingBuffer) {
	stagingBuffersToDelete.Insert(stagingBuffer);
}

void ICommandList::DeleteAllStagingBuffers() {
	for (TSize i = 0; i < stagingBuffersToDelete.GetSize(); i++)
		stagingBuffersToDelete.At(i).Delete();

	stagingBuffersToDelete.Free();
}
