#pragma once

#include "Wrapper.h"

#include <OSKengine/SoundEntity.h>

namespace OSKengine {

	public ref class SoundEntity : public Wrapper<OSK::SoundEntity> {

	public:

		SoundEntity();

		void SetPosition(Vector3f position);

		void SetDirection(Vector3 direction);

		void SetSpeed(Vector3 speed);

		void SetPitch(float pitch);

		void SetGain(float gain);
	};

}