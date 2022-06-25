#include "IBottomLevelAccelerationStructure.h"

#include "Assert.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void IBottomLevelAccelerationStructure::SetMatrix(const glm::mat4& matrix) {
	OSK_ASSERT(matrixBuffer.HasValue(), "No se inició el buffer de la matrix.");

	matrixBuffer->MapMemory();
	matrixBuffer->Write(glm::transpose(matrix));
	matrixBuffer->Unmap();

	isDirty = true;
}

bool IBottomLevelAccelerationStructure::IsDirty() const {
	return isDirty;
}
