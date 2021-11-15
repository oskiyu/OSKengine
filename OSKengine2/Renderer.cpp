#include "IRenderer.h"

using namespace OSK;

ICommandList* IRenderer::GetCommandList() const {
	return commandList.GetPointer();
}
