#include "AudioApi.h"

#include <AL/al.h>
#include <array>

#include "Assert.h"
#include "AudioExceptions.h"
#include "Transform3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::AUDIO;

AudioApi::AudioApi() {
	FindDevices();
}


void AudioApi::SetListener(const Transform3D& listenerTransform) {
	SetListenerPosition(listenerTransform.GetPosition());
	SetListenerOrientation(listenerTransform.GetForwardVector(), listenerTransform.GetTopVector());
}

void AudioApi::SetListenerPosition(const Vector3f& position) {
	alListener3f(
		AL_POSITION,
		position.x,
		position.y,
		position.Z
	);
}

void AudioApi::SetListenerOrientation(const Vector3f& forward, const Vector3f& up) {
	const std::array<float, 6> values = {
		forward.x,
		forward.y,
		forward.Z,

		up.x,
		up.y,
		up.Z
	};

	alListenerfv(
		AL_ORIENTATION,
		values.data()
	);
}

void AudioApi::SetListenerVelocity(const Vector3f& velocity) {
	alListener3f(
		AL_VELOCITY,
		velocity.x,
		velocity.y,
		velocity.Z
	);
}

const Device& AudioApi::GetCurrentDevice() const {
	return devices.at(currentDeviceName);
}

const Device& AudioApi::GetDevice(std::string_view name) const {
	return devices.at((std::string)name);
}

void AudioApi::SetCurrentDevice(std::string_view name) {
	currentDeviceName = (std::string)name;
	devices.at(currentDeviceName).SetCurrent();
}

const std::unordered_map<std::string, Device>& AudioApi::GetAllDevices() const {
	return devices;
}

void AudioApi::FindDevices() {
	const ALCchar* devicesNames = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

	do {
		const auto name = std::string(devicesNames);
		devicesNames += name.size() + 1;

		devices[name] = Device(name);
	} while (devicesNames[1] != NULL);
	
	defaultDeviceName = std::string(alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
	currentDeviceName = defaultDeviceName;
	
	devices.at(defaultDeviceName).SetCurrent();

	OSK_ASSERT(alcGetCurrentContext() != nullptr, AudioDeviceCreationException("No se pudo establecer un contexto."))
}
