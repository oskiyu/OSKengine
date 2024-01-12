#include "AudioSourceAl.h"

#include <AL/al.h>
#include "Assert.h"
#include "AudioExceptions.h"

using namespace OSK;
using namespace OSK::AUDIO;

AudioSourceAl::~AudioSourceAl() {
	if (m_handle != EMPTY_HANDLE) {
		alDeleteSources(1, &m_handle);
	}
}

void AudioSourceAl::Initialize() {
	alGenSources(1, &m_handle);

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al generar el source.", result));
}

void AudioSourceAl::SetBuffer(const Buffer& buffer) {
	alSourcei(m_handle, AL_BUFFER, buffer.GetHandle());

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al establecer el buffer.", result));
}

void AudioSourceAl::Play_Implementation() {
	alSourcePlay(m_handle);
}

void AudioSourceAl::Pause_Implementation() {
	alSourcePause(m_handle);
}

void AudioSourceAl::Stop_Implementation() {
	alSourceStop(m_handle);
}

void AudioSourceAl::SetPosition(const Vector3f& position) {
	alSource3f(m_handle, AL_POSITION, position.x, position.y, position.z);
}

void AudioSourceAl::SetVelocity(const Vector3f& velocity) {
	alSource3f(m_handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void AudioSourceAl::SetPitch_Implementation(float pitch) {
	alSourcef(m_handle, AL_PITCH, pitch);
}

void AudioSourceAl::SetGain_Implementation(float gain) {
	alSourcef(m_handle, AL_GAIN, gain);
}

void AudioSourceAl::SetLooping_Implementation(bool looping) {
	alSourcei(m_handle, AL_LOOPING, looping);
}
