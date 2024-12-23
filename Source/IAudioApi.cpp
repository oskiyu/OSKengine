#include "IAudioApi.h"

#include "TransformComponent3D.h"

#include <format>

#include "Assert.h"
#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::AUDIO;


void IAudioApi::SetListener(const Transform3D& listenerTransform) {
	SetListenerPosition(listenerTransform.GetPosition());
	SetListenerOrientation(listenerTransform.GetForwardVector(), listenerTransform.GetTopVector());
}

const IAudioDevice& IAudioApi::GetCurrentDevice() const {
	return m_devices.find(m_currentDeviceName)->second.GetValue();
}

const IAudioDevice& IAudioApi::GetDevice(std::string_view name) const {
	OSK_ASSERT(m_devices.contains(name),
		InvalidArgumentException(std::format("No existe un dispositivo con nombre {}", name)));

	return m_devices.find(name)->second.GetValue();
}

void IAudioApi::SetCurrentDevice(std::string_view name) {
	OSK_ASSERT(m_devices.contains(name),
		InvalidArgumentException(std::format("No existe un dispositivo con nombre {}", name)));

	auto* device = m_devices.find(name)->second.GetPointer();

	device->SetCurrentOutputDevice();

	m_currentDeviceName = device->GetName();
}

const std::unordered_map<std::string, UniquePtr<IAudioDevice>, StringHasher, std::equal_to<>>& IAudioApi::GetAllDevices() const {
	return m_devices;
}

void IAudioApi::RegisterDevice(OwnedPtr<IAudioDevice> device) {
	m_devices[static_cast<std::string>(device->GetName())] = device.GetPointer();
}

void IAudioApi::SetDefaultDeviceName(const std::string& name) {
	m_defaultDeviceName = name;
}
