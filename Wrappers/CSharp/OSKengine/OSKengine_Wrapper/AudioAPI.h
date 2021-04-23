#pragma once

#include "Wrapper.h"

#include <OSKengine/AudioAPI.h>

#include "Vector2f.h"
#include "Vector3.hpp"

#include "SoundEntity.h"

namespace OSKengine {

	public ref class AudioAPI : public Wrapper<OSK::AudioSystem> {

	public:

		AudioAPI();

/*		void Update();

		void SetCamera3D(Camera3D% camera);

		void SetListenerSpeed(Vector3<float> speed);

		void PlayAudio3D(SoundEmitterComponent% audio, bool bucle);
		void PlayAudio(SoundEmitterComponent% audio, bool bucle);

		void PauseAudio(SoundEmitterComponent% audio);

		void StopAudio(SoundEmitterComponent% audio);

		void RestartAudio(SoundEmitterComponent% audio);*/

	};

}