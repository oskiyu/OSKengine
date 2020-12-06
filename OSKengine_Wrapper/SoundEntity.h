#pragma once

#include "Wrapper.h"

#include <OSKengine/SoundEntity.h>

#include "Vector3.hpp"

namespace OSKengine {

	public ref class SoundEntity : public Wrapper<OSK::SoundEntity> {

	public:

		SoundEntity() : Wrapper(new OSK::SoundEntity) {}
		/*
		void SetPosition(Vector3<float> position);

		void SetDirection(Vector3<float> direction);

		void SetSpeed(Vector3<float> speed);

		void SetPitch(float pitch);

		void SetGain(float gain);*/
	};

}