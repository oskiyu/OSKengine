#include "IAudioSource.h"

using namespace OSK;
using namespace OSK::AUDIO;


void IAudioSource::Initialize() {
	(void)0;
}

void IAudioSource::SetPitch(float pitch) {
	m_pitch = pitch;
}

void IAudioSource::SetGain(float gain) {
	m_gain = gain;
}

void IAudioSource::SetLooping(bool looping) {
	m_loop = looping;
}

void IAudioSource::Play() {
	Play_Implementation();
}

void IAudioSource::Pause() {
	Pause_Implementation();
}

void IAudioSource::Stop() {
	Stop_Implementation();
}

void IAudioSource::SetCurrentState(AudioState state) {
	m_state = state;

	switch (state) {
	case AudioState::PLAYING:
		Play();
		break;
	case AudioState::PAUSED:
		Pause();
		break;
	case AudioState::STOPPED:
		Stop();
		break;

	case AudioState::UNKNOWN:
		break;
	}
}

AudioState IAudioSource::GetCurrentState() const {
	return m_state;
}
