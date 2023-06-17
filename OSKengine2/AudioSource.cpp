#include "AudioSource.h"

#include <AL/al.h>
#include "Assert.h"
#include "AudioExceptions.h"

using namespace OSK;
using namespace OSK::AUDIO;

Source::Source() {

}

Source::~Source() {
	if (handle != EMPTY_HANDLE)
		alDeleteSources(1, &handle);
}

void Source::Init() {
	alGenSources(1, &handle);

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al generar el source.", result));
}

void Source::SetBuffer(const Buffer& buffer) {
	alSourcei(handle, AL_BUFFER, buffer.GetHandle());

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al establecer el buffer.", result));
}

Source::State Source::GetCurrentState() const {
	ALint state = 0;
	alGetSourcei(handle, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
		return State::PLAYING;
	else if (state == AL_PAUSED)
		return State::PAUSED;
	else if (state == AL_INITIAL || state == AL_STOPPED)
		return State::STOPPED;
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
	alSourcePlay(handle);
}

void Source::Pause() {
	alSourcePause(handle);
}

void Source::Stop() {
	alSourceStop(handle);
}

void Source::SetPosition(const Vector3f& position) {
	alSource3f(handle, AL_POSITION, position.x, position.y, position.Z);
}

void Source::SetVelocity(const Vector3f& velocity) {
	alSource3f(handle, AL_VELOCITY, velocity.x, velocity.y, velocity.Z);
}

void Source::SetPitch(float pitch) {
	this->pitch = pitch;
	alSourcef(handle, AL_PITCH, pitch);
}

void Source::SetGain(float gain) {
	this->gain = gain;
	alSourcef(handle, AL_GAIN, gain);
}

void Source::SetLooping(bool looping) {
	alSourcei(handle, AL_LOOPING, looping);
}
