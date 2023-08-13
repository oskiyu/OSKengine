#include "AudioAPI.h"

#include <al.h>
#include "../OSKengine2/vcpkg_installed/x64-windows/x64-windows/include/AL/alc.h"

namespace OSK {

	void AudioSystem::OnCreate() {
		device = alcOpenDevice(NULL);
		if (!device.HasValue()) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "crear dispositivo de audo", __LINE__);
			return;
		}

		context = alcCreateContext(device.GetPointer(), NULL);
		if (!context.HasValue()) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "crear contexto de audo", __LINE__);
			return;
		}

		alcMakeContextCurrent(context.GetPointer());
	}


	void AudioSystem::OnRemove() {
		if (!context.HasValue()) {
			alcMakeContextCurrent(NULL);
			alcDestroyContext(context.GetPointer());
		}
		if (!device.HasValue()) {
			alcCloseDevice(device.GetPointer());
		}
	}


	void AudioSystem::OnTick(deltaTime_t deltaTime) {
		if (camera == nullptr)
			return;

		//Posición.
		alListener3f(AL_POSITION, camera->GetTransform()->GetPosition().X, camera->GetTransform()->GetPosition().Y, camera->GetTransform()->GetPosition().Z);

		//Dirección.
		float_t direction[6] = { 
			camera->GetTransform()->GetForwardVector().X, 
			camera->GetTransform()->GetForwardVector().Y, 
			camera->GetTransform()->GetForwardVector().Z, 
			
			camera->GetTransform()->GetTopVector().X, 
			camera->GetTransform()->GetTopVector().Y, 
			camera->GetTransform()->GetTopVector().Z 
		};

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

		int b = AL_TRUE;
		if (!bucle)
			b = AL_FALSE;

		alSourcei(audio.sourceID, AL_LOOPING, b);
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
