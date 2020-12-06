#pragma once

#include "Wrapper.h"

#include <OSKengine/AudioAPI.h>

#include "Vector2f.h"
#include "Vector3.hpp"

#include "SoundEntity.h"

namespace OSKengine {

	public ref class AudioAPI : public Wrapper<OSK::AudioAPI> {

	public:

		AudioAPI();

/*		void Update();

		void SetCamera3D(Camera3D% camera);

		void SetListenerSpeed(Vector3<float> speed);

		void PlayAudio3D(SoundEntity% audio, bool bucle);
		void PlayAudio(SoundEntity% audio, bool bucle);

		void PauseAudio(SoundEntity% audio);

		void StopAudio(SoundEntity% audio);

		void RestartAudio(SoundEntity% audio);*/

	};

}