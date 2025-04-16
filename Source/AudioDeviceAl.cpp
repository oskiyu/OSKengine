#include "AudioDeviceAl.h"

#include "Assert.h"
#include "AudioExceptions.h"

using namespace OSK;
using namespace OSK::AUDIO;

void AudioDeviceAl::Initialize(const std::string& name) {
	m_device = UniquePtr<ALCdevice, ALCdeviceDeleter>(alcOpenDevice(name.data()));
	OSK_ASSERT(m_device.HasValue(), AudioDeviceCreationException("Error al crear el device."));

	m_context = UniquePtr<ALCcontext, ALCcontextDeleter>(alcCreateContext(m_device.GetPointer(), nullptr));
	OSK_ASSERT(m_context.HasValue(), AudioDeviceCreationException("Error al crear el contexto."));

	SetName(name);
}

void AudioDeviceAl::InitializeDefault() {
	m_device = UniquePtr<ALCdevice, ALCdeviceDeleter>(alcOpenDevice(nullptr));
	m_context = UniquePtr<ALCcontext, ALCcontextDeleter>(alcCreateContext(m_device.GetPointer(), nullptr));

	SetName("Default device");
}

AudioDeviceAl::~AudioDeviceAl() {
	if (IsCurrentOutputDevice()) {
		alcMakeContextCurrent(nullptr);
	}
}

void AudioDeviceAl::ALCdeviceDeleter::operator()(ALCdevice* device) const noexcept {
	if (device) {
		alcCloseDevice(device);
	}
}

void AudioDeviceAl::ALCcontextDeleter::operator()(ALCcontext* context) const noexcept {
	if (context){
		alcDestroyContext(context);
	}
}

struct ALCcontextDeleter {
	void operator()(ALCcontext*) const noexcept;
};


void AudioDeviceAl::SetCurrentOutputDevice_Implementation() {
	const auto result = alcMakeContextCurrent(m_context.GetPointer());
	OSK_ASSERT(result == ALC_TRUE, AudioException("Error al establecer el contexto.", alcGetError(m_device.GetPointer())));
}
