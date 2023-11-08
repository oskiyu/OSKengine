#include "AudioSource.h"

#include <AL/al.h>
#include "Assert.h"
#include "AudioExceptions.h"

using namespace OSK;
using namespace OSK::AUDIO;

Source::~Source() {
	if (m_handle != EMPTY_HANDLE)
		alDeleteSources(1, &m_handle);
}

void Source::Init() {
	alGenSources(1, &m_handle);

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al generar el source.", result));
}

void Source::SetBuffer(const Buffer& buffer) {
	alSourcei(m_handle, AL_BUFFER, buffer.GetHandle());

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al establecer el buffer.", result));
}

Source::State Source::GetCurrentState() const {
	ALint state = 0;
	alGetSourcei(m_handle, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
		return State::PLAYING;
	else if (state == AL_PAUSED)
		return State::PAUSED;
	else if (state == AL_INITIAL || state == AL_STOPPED)
		return State::STOPPED;

	return State::UNKNOWN;
}

void Source::SetCurrentState(State state) {
	switch (state) {
	case OSK::AUDIO::Source::State::PLAYING:
		Play();
		break;
	case OSK::AUDIO::Source::State::PAUSED:
		Pause();
		break;
	case OSK::AUDIO::Source::State::STOPPED:
		Stop();
		break;
	}
}

void Source::Play() {
	alSourcePlay(m_handle);
}

void Source::Pause() {
	alSourcePause(m_handle);
}

void Source::Stop() {
	alSourceStop(m_handle);
}

void Source::SetPosition(const Vector3f& position) {
	alSource3f(m_handle, AL_POSITION, position.x, position.y, position.z);
}

void Source::SetVelocity(const Vector3f& velocity) {
	alSource3f(m_handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void Source::SetPitch(float pitch) {
	m_pitch = pitch;
	alSourcef(m_handle, AL_PITCH, pitch);
}

void Source::SetGain(float gain) {
	m_gain = gain;
	alSourcef(m_handle, AL_GAIN, gain);
}

void Source::SetLooping(bool looping) {
	alSourcei(m_handle, AL_LOOPING, looping);
}
