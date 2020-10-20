#include "AudioAPI.h"

#include <al.h>

namespace OSK {

	AudioAPI::AudioAPI() {
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


	AudioAPI::~AudioAPI() {
		if (context != nullptr) {
			alcMakeContextCurrent(NULL);
			alcDestroyContext(context);
		}
		if (device != nullptr) {
			alcCloseDevice(device);
		}
	}


	void AudioAPI::Update() {
		if (camera == nullptr)
			return;

		//Posición.
		alListener3f(AL_POSITION, camera->CameraTransform.GlobalPosition.X, camera->CameraTransform.GlobalPosition.Y, camera->CameraTransform.GlobalPosition.Z);

		//Dirección.
		float_t direction[6] = { camera->Front.X, camera->Front.Y, camera->Front.Z, camera->Up.X, camera->Up.Y, camera->Up.Z };
		alListenerfv(AL_ORIENTATION, direction);
	}


	void AudioAPI::SetCamera3D(Camera3D* camera) {
		this->camera = camera;
	}


	void AudioAPI::SetListenerSpeed(const Vector3& speed) {
		alListener3f(AL_VELOCITY, speed.X, speed.Y, speed.Z);
	}


	void AudioAPI::PlayAudio3D(SoundEntity& audio, const bool& bucle) {
		audio.SetPosition(audio.SoundTransform.GlobalPosition);
		alSourcei(audio.SourceID, AL_LOOPING, bucle);
		alSourcePlay(audio.SourceID);
	}


	void AudioAPI::PlayAudio(SoundEntity& audio, const bool& bucle) {
		if (camera != nullptr)
			audio.SetPosition(camera->CameraTransform.GlobalPosition);

		alSourcei(audio.SourceID, AL_LOOPING, bucle);
		alSourcePlay(audio.SourceID);
	}


	void AudioAPI::PauseAudio(const SoundEntity& audio) {
		alSourcePause(audio.SourceID);
	}


	void AudioAPI::StopAudio(const SoundEntity& audio) {
		alSourcePause(audio.SourceID);
	}


	void AudioAPI::RestartAudio(const SoundEntity& audio) {
		alSourceStop(audio.SourceID);
		alSourcePlay(audio.SourceID);
	}

}