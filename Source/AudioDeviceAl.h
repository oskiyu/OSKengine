#pragma once

#include "ApiCall.h"
#include "OwnedPtr.h"

#include "IAudioDevice.h"

#include <AL/alc.h>
#include <string>

namespace OSK::AUDIO {

	/// @brief Clase que representa un dispositivo de salida de audio.
	class OSKAPI_CALL AudioDeviceAl : public IAudioDevice {

	public:

		~AudioDeviceAl() override;

		void InitializeDefault() override;
		void Initialize(const std::string& name) override;

	protected:

		void SetCurrentOutputDevice_Implementation() override;

	private:

		/// @brief Dispositivo de audio.
		OwnedPtr<ALCdevice> m_device = nullptr;

		/// @brief Contexto del dispositivo.
		/// Contiene los buffers y los sources,
		/// además de un listener.
		OwnedPtr<ALCcontext> m_context = nullptr;

	};

}
