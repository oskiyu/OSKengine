#include "SoundEntity.h"

#include <al.h>

namespace OSK {

	void SoundEmitterComponent::OnCreate() {
		
	}


	void SoundEmitterComponent::OnRemove() {
		alDeleteSources(1, &SourceID);
		alDeleteBuffers(1, &BufferID);
	}


	void SoundEmitterComponent::SetPosition(const Vector3& position) {
		alSource3f(SourceID, AL_POSITION, position.X, position.Y, position.Z);
	}


	void SoundEmitterComponent::SetDirection(const Vector3& direction) {
		float_t _direction[3] = { direction.X, direction.Y, direction.Z };
		alSourcefv(SourceID, AL_DIRECTION, _direction);
	}


	void SoundEmitterComponent::SetSpeed(const Vector3& speed) {
		alSource3f(SourceID, AL_VELOCITY, speed.X, speed.Y, speed.Z);
	}


	void SoundEmitterComponent::SetPitch(float ptich) {
		alSourcef(SourceID, AL_PITCH, ptich);
	}


	void SoundEmitterComponent::SetGain(float gain) {
		alSourcef(SourceID, AL_GAIN, gain);
	}

}
