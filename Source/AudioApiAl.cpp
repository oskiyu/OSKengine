#include "AudioApiAl.h"

#include "AudioDeviceAl.h"

#include <AL/al.h>
#include <array>

#include "Assert.h"
#include "AudioExceptions.h"
#include "TransformComponent3D.h"

#include "AudioSourceAl.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::AUDIO;

void AudioApiAl::Initialize() {
	RegisterAllDevices();
}


void AudioApiAl::SetListenerPosition(const Vector3f& position) {
	alListener3f(
		AL_POSITION,
		position.x,
		position.y,
		position.z
	);
}

void AudioApiAl::SetListenerOrientation(const Vector3f& forward, const Vector3f& up) {
	const std::array<float, 6> values = {
		forward.x,
		forward.y,
		forward.z,

		up.x,
		up.y,
		up.z
	};

	alListenerfv(
		AL_ORIENTATION,
		values.data()
	);
}

void AudioApiAl::SetListenerVelocity(const Vector3f& velocity) {
	alListener3f(
		AL_VELOCITY,
		velocity.x,
		velocity.y,
		velocity.z
	);
}

void AudioApiAl::RegisterAllDevices() {
	const ALCchar* devicesNames = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

	do {
		const auto name = std::string(devicesNames);
		devicesNames += name.size() + 1;

		auto* newDevice = new AudioDeviceAl();
		newDevice->Initialize(name);

		RegisterDevice(newDevice);
	} while (devicesNames[1] != NULL);
	
	auto defaultDeviceName = std::string(alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
	
	SetDefaultDeviceName(defaultDeviceName);
	SetCurrentDevice(defaultDeviceName);

	OSK_ASSERT(alcGetCurrentContext() != nullptr, AudioDeviceCreationException("No se pudo establecer un contexto."))
}

OwnedPtr<IAudioSource> AudioApiAl::CreateNewSource() const {
	return new AudioSourceAl;
}
