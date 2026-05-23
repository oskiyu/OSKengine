#include "ITouchInput.h"

using namespace OSK;
using namespace OSK::IO;

const DynamicArray<TouchInput>& ITouchInput::GetCurrentFrameInputs() const {
	return m_currentFrameInputs;
}
