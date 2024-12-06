#include "IBottomLevelAccelerationStructure.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "Assert.h"
#include "AccelerationStructuresExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void IBottomLevelAccelerationStructure::SetMatrix(const glm::mat4& matrix) {
	OSK_ASSERT(matrixBuffer.HasValue(), MatrixBufferNotCreatedException());

	matrixBuffer->MapMemory();
	matrixBuffer->Write(glm::transpose(matrix));
	matrixBuffer->Unmap();

	isDirty = true;
}

bool IBottomLevelAccelerationStructure::IsDirty() const {
	return isDirty;
}

#endif
