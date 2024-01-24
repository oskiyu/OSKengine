#include "AudioDeviceAl.h"

#include "Assert.h"
#include "AudioExceptions.h"

using namespace OSK;
using namespace OSK::AUDIO;

void AudioDeviceAl::Initialize(const std::string& name) {
	m_device = alcOpenDevice(name.data());
	OSK_ASSERT(m_device != nullptr, AudioDeviceCreationException("Error al crear el device."));

	m_context = alcCreateContext(m_device.GetPointer(), nullptr);
	OSK_ASSERT(m_context != nullptr, AudioDeviceCreationException("Error al crear el contexto."));

	SetName(name);
}

void AudioDeviceAl::InitializeDefault() {
	m_device = alcOpenDevice(nullptr);
	m_context = alcCreateContext(m_device.GetPointer(), nullptr);

	SetName("Default device");
}

AudioDeviceAl::~AudioDeviceAl() {
	if (IsCurrentOutputDevice())
		alcMakeContextCurrent(nullptr);

	if (m_context.HasValue())
		alcDestroyContext(m_context.GetPointer());

	if (m_device.GetPointer())
		alcCloseDevice(m_device.GetPointer());

	m_context = nullptr;
	m_device = nullptr;
}


void AudioDeviceAl::SetCurrentOutputDevice_Implementation() {
	const auto result = alcMakeContextCurrent(m_context.GetPointer());
	OSK_ASSERT(result == ALC_TRUE, AudioException("Error al establecer el contexto.", alcGetError(m_device.GetPointer())));
}
