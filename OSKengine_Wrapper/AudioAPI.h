#pragma once

#include "Wrapper.h"

#include <OSKengine/AudioAPI.h>

namespace OSKengine {

	public ref class AudioAPI : public Wrapper<OSK::AudioAPI> {

	public:

		AudioAPI();

		void Update();

		void SetCamera3D(Camera3D% camera);

		void SetListenerSpeed(Vector3f speed);

		void PlayAudio3D(SoundEntity% audio, bool bucle);
		void PlayAudio(SoundEntity% audio, bool bucle);

		void PauseAudio(SoundEntity% audio);

		void StopAudio(SoundEntity% audio);

		void RestartAudio(SoundEntity% audio);

	};

}