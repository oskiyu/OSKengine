#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"

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

		struct ALCdeviceDeleter {
			void operator()(ALCdevice*) const noexcept;
		};

		struct ALCcontextDeleter {
			void operator()(ALCcontext*) const noexcept;
		};

		/// @brief Dispositivo de audio.
		UniquePtr<ALCdevice, ALCdeviceDeleter> m_device;

		/// @brief Contexto del dispositivo.
		/// Contiene los buffers y los sources,
		/// además de un listener.
		UniquePtr<ALCcontext, ALCcontextDeleter> m_context;

	};

}
