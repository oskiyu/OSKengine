#include "SoundEntity.h"

#include <al.h>

namespace OSK {

	SoundEntity::SoundEntity() {
		SoundTransform = Transform();
	}


	SoundEntity::~SoundEntity() {
		alDeleteSources(1, &SourceID);
		alDeleteBuffers(1, &BufferID);
	}


	void SoundEntity::SetPosition(const Vector3& position) {
		alSource3f(SourceID, AL_POSITION, position.X, position.Y, position.Z);
	}


	void SoundEntity::SetDirection(const Vector3& direction) {
		float_t _direction[3] = { direction.X, direction.Y, direction.Z };
		alSourcefv(SourceID, AL_DIRECTION, _direction);
	}


	void SoundEntity::SetSpeed(const Vector3& speed) {
		alSource3f(SourceID, AL_VELOCITY, speed.X, speed.Y, speed.Z);
	}


	void SoundEntity::SetPitch(const float_t& ptich) {
		alSourcef(SourceID, AL_PITCH, ptich);
	}


	void SoundEntity::SetGain(const float_t& gain) {
		alSourcef(SourceID, AL_GAIN, gain);
	}

}