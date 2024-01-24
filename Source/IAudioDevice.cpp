#include "IAudioDevice.h"

using namespace OSK;
using namespace OSK::AUDIO;


void IAudioDevice::SetCurrentOutputDevice() {
	SetCurrentOutputDevice_Implementation();
	m_isCurrentOutputDevice = true;
}

bool IAudioDevice::IsCurrentOutputDevice() const {
	return m_isCurrentOutputDevice;
}

void IAudioDevice::SetName(const std::string& name) {
	m_name = name;
}
