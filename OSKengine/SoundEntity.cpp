#include "SoundEntity.h"

#include <al.h>

namespace OSK {

	void SoundEmitterComponent::OnCreate() {
		
	}


	void SoundEmitterComponent::OnRemove() {
		alDeleteSources(1, &sourceID);
		alDeleteBuffers(1, &bufferID);
	}


	void SoundEmitterComponent::SetPosition(const Vector3& position) {
		alSource3f(sourceID, AL_POSITION, position.X, position.Y, position.Z);
	}


	void SoundEmitterComponent::SetDirection(const Vector3& direction) {
		float_t _direction[3] = { direction.X, direction.Y, direction.Z };
		alSourcefv(sourceID, AL_DIRECTION, _direction);
	}


	void SoundEmitterComponent::SetSpeed(const Vector3& speed) {
		alSource3f(sourceID, AL_VELOCITY, speed.X, speed.Y, speed.Z);
	}


	void SoundEmitterComponent::SetPitch(float ptich) {
		alSourcef(sourceID, AL_PITCH, ptich);
	}


	void SoundEmitterComponent::SetGain(float gain) {
		alSourcef(sourceID, AL_GAIN, gain);
	}

}
