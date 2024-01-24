#include "ITopLevelAccelerationStructure.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ITopLevelAccelerationStructure::AddBottomLevelAccelerationStructure(IBottomLevelAccelerationStructure* blas) {
	blass.Insert(blas);
	needsRebuild = true;
}
