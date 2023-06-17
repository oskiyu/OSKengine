#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"

#include <AL/alc.h>
#include <string>

namespace OSK::AUDIO {

	/// @brief Clase que representa un dispositivo de salida de audio.
	class OSKAPI_CALL Device {

	public:

		~Device();

		OSK_DISABLE_COPY(Device)
		OSK_DEFAULT_MOVE_OPERATOR(Device)

		/// @brief Instancia un objeto que referencia al dispositivo de salida
		/// de audio por defdecto del sistema.
		/// 
		/// @throws AudioDeviceCreationException Si ocurre algún error al crear el device.
		Device();

		/// @brief Instancia un objeto que referencia a un dispositivo en concreto.
		/// @param name Nombre del dispositivo.
		explicit Device(const std::string& name);


		/// @return Nombre del dispositivo.
		inline std::string_view GetName() const { return name; }

		/// @brief Establece este dispositivo como el que se va a usar para la salida
		/// de audio del juego.
		/// 
		/// @throws AudioDeviceCreationException Si ocurre algún error al establecer el 
		/// device por defecto.
		void SetCurrent();

		/// @return True si este dispositivo es el que se está usando como salida de audio
		/// del juego.
		bool IsCurrent() const;

	private:

		/// @brief Dispositivo de audio.
		OwnedPtr<ALCdevice> device = nullptr;

		/// @brief Contexto del dispositivo.
		/// Contiene los buffers y los sources,
		/// además de un listener.
		OwnedPtr<ALCcontext> context = nullptr;

		/// @brief Nombre del dispositivo.
		std::string name;

	};

}
