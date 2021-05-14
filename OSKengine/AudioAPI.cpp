#include "AudioAPI.h"

#include <al.h>

namespace OSK {

	void AudioSystem::OnCreate() {
		device = alcOpenDevice(NULL);
		if (device == NULL) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "crear dispositivo de audo", __LINE__);
			return;
		}

		context = alcCreateContext(device, NULL);
		if (context == NULL) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "crear contexto de audo", __LINE__);
			return;
		}

		alcMakeContextCurrent(context);
	}


	void AudioSystem::OnRemove() {
		if (context != nullptr) {
			alcMakeContextCurrent(NULL);
			alcDestroyContext(context);
		}
		if (device != nullptr) {
			alcCloseDevice(device);
		}
	}


	void AudioSystem::OnTick(deltaTime_t deltaTime) {
		if (camera == nullptr)
			return;

		//Posición.
		alListener3f(AL_POSITION, camera->GetTransform()->GetPosition().X, camera->GetTransform()->GetPosition().Y, camera->GetTransform()->GetPosition().Z);

		//Dirección.
		float_t direction[6] = { camera->GetFrontVector().X, camera->GetFrontVector().Y, camera->GetFrontVector().Z, camera->GetUpVector().X, camera->GetUpVector().Y, camera->GetUpVector().Z };
		alListenerfv(AL_ORIENTATION, direction);
	}


	void AudioSystem::SetCamera3D(Camera3D* camera) {
		this->camera = camera;
	}


	void AudioSystem::SetListenerSpeed(const Vector3& speed) {
		alListener3f(AL_VELOCITY, speed.X, speed.Y, speed.Z);
	}


	void AudioSystem::PlayAudio3D(SoundEmitterComponent& audio, bool bucle) {
		//audio.SetPosition(audio.SoundTransform.GlobalPosition);
		alSourcei(audio.sourceID, AL_LOOPING, bucle);
		alSourcePlay(audio.sourceID);
	}


	void AudioSystem::PlayAudio(SoundEmitterComponent& audio, bool bucle) {
		if (camera != nullptr)
			audio.SetPosition(camera->GetTransform()->GetPosition());

		alSourcei(audio.sourceID, AL_LOOPING, bucle);
		alSourcePlay(audio.sourceID);
	}


	void AudioSystem::PauseAudio(const SoundEmitterComponent& audio) {
		alSourcePause(audio.sourceID);
	}


	/*void AudioSystem::StopAudio(const SoundEmitterComponent& audio) {
		alSourcePause(audio.SourceID);
	}*/


	void AudioSystem::RestartAudio(const SoundEmitterComponent& audio) {
		alSourceStop(audio.sourceID);
		alSourcePlay(audio.sourceID);
	}

}