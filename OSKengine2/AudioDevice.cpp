#include "AudioDevice.h"

#include "Assert.h"
#include "AudioExceptions.h"

using namespace OSK;
using namespace OSK::AUDIO;

Device::Device(const std::string& name) : name(name) {
	device = alcOpenDevice(name.data());
	OSK_ASSERT(device != nullptr, AudioDeviceCreationException("Error al crear el device."));

	context = alcCreateContext(device.GetPointer(), nullptr);
	OSK_ASSERT(context != nullptr, AudioDeviceCreationException("Error al crear el contexto."));
}

Device::Device() : name("DEFAULT") {
	device = alcOpenDevice(nullptr);
	context = alcCreateContext(device.GetPointer(), nullptr);
}

Device::~Device() {
	if (IsCurrent())
		alcMakeContextCurrent(nullptr);

	if (context.HasValue())
		alcDestroyContext(context.GetPointer());

	if (device.GetPointer())
		alcCloseDevice(device.GetPointer());

	context = nullptr;
	device = nullptr;
}


void Device::SetCurrent() {
	const auto result = alcMakeContextCurrent(context.GetPointer());
	OSK_ASSERT(result == ALC_TRUE, AudioException("Error al establecer el contexto.", alcGetError(device.GetPointer())));
}

bool Device::IsCurrent() const {
	return alcGetCurrentContext() == context.GetPointer();
}
