#pragma once

#include "IAudioApi.h"

#include <AL/alc.h>

namespace OSK::AUDIO {

	/// @brief Clase del sistema que permite la reproducción
	/// de audio 3D.
	class OSKAPI_CALL AudioApiAl : public IAudioApi {

	public:

		AudioApiAl() = default;

		void Initialize() override;

		void SetListenerPosition(const Vector3f& position) override;
		void SetListenerOrientation(const Vector3f& forward, const Vector3f& up) override;
		void SetListenerVelocity(const Vector3f& velocity) override;

		UniquePtr<IAudioSource> CreateNewSource() const override;

	protected:

		void RegisterAllDevices() override;

	};

}
