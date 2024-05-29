#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include <string>
#include <string_view>

namespace OSK::AUDIO {

	class OSKAPI_CALL IAudioDevice {

	public:
		
		virtual ~IAudioDevice() = default;

		OSK_DEFINE_AS(IAudioDevice);


		/// @brief Instancia un objeto que referencia al dispositivo de salida
		/// de audio por defdecto del sistema.
		/// 
		/// @throws AudioDeviceCreationException Si ocurre algún error al crear el device.
		virtual void InitializeDefault() = 0;

		/// @brief Instancia un objeto que referencia a un dispositivo en concreto.
		/// @param name Nombre del dispositivo.
		virtual void Initialize(const std::string& name) = 0;


		/// @return Nombre del dispositivo.
		inline std::string_view GetName() const { return m_name; }


		/// @brief Establece este dispositivo como el que se va a usar para la salida
		/// de audio del juego.
		/// 
		/// @throws AudioDeviceCreationException Si ocurre algún error al establecer el 
		/// device por defecto.
		void SetCurrentOutputDevice();

		/// @return True si este dispositivo es el que se está usando como salida de audio
		/// del juego.
		bool IsCurrentOutputDevice() const;


	protected:

		virtual void SetCurrentOutputDevice_Implementation() = 0;

		void SetName(const std::string& name);

	private:

		/// @brief Nombre del dispositivo.
		std::string m_name;

		bool m_isCurrentOutputDevice = false;

	};

}
