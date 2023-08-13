#pragma once

#include "OSKmacros.h"

#include "AudioDevice.h"
#include "HashMap.hpp"

#include "Vector3.hpp"

#include <AL/alc.h>

namespace OSK::ECS { class Transform3D; }

namespace OSK::AUDIO {

	/// @brief Clase del sistema que permite la reproducción
	/// de audio 3D.
	class OSKAPI_CALL AudioApi {

	public:

		OSK_DEFAULT_MOVE_OPERATOR(AudioApi);

		/// @brief Inicializa la API de bajo nivel y realiza una búsqueda
		/// de los dispositivos de salida disponibles.
		/// @throws AudioDeviceCreationException Si ocurre algún error al crear un device.
		/// @throws AudioDeviceCreationException Si ocurre algún error al establecer el 
		/// device por defecto.
		AudioApi();

		OSK_DISABLE_COPY(AudioApi)

		/// @brief Establece la posición y orientación del jugador (listener).
		/// @param listenerTransform Transform del jugador.
		/// @see SetListenerPosition.
		/// @see SetListenerOrientation.
		void SetListener(const ECS::Transform3D& listenerTransform);

		/// @brief Establece la posición del jugador.
		/// @param position Posición del jugador en el mundo 3D.
		void SetListenerPosition(const Vector3f& position);

		/// @brief Establece la orientación del jugador.
		/// @param forward Vector hacia delante del jugador.
		/// @param up Vector hacia arriba del jugador.
		void SetListenerOrientation(const Vector3f& forward, const Vector3f& up);

		/// @brief Establece la velocidad a la que se mueve el jugador en el 
		/// mundo 3D.
		/// @param velocity Velocidad del jugador.
		void SetListenerVelocity(const Vector3f& velocity);


		/// @return Devuelve una referencia al dispositivo de
		/// salida que se está usando en un momento dado.
		const Device& GetCurrentDevice() const;

		/// @param name Nombre del dispositivo.
		/// @return Dispositivo de salida con el nombre @p name.
		/// @pre Debe haber un dispositivo cuyo nombre sea @p name.
		const Device& GetDevice(std::string_view name) const;

		/// @brief Establece el dispositivo de salida a usar.
		/// @param name Nombre del dispositivo deseado.
		/// @pre Debe haber un dispositivo cuyo nombre sea @p name.
		void SetCurrentDevice(std::string_view name);

		/// @return Todos los dispositivos disponibles.
		const std::unordered_map<std::string, Device>& GetAllDevices() const;

	private:

		/// @brief Busca todos los dispositivos de salida disponibles,
		/// y los almacena en la variable @p devices.
		/// @throws AudioDeviceCreationException Si ocurre algún error al crear un device.
		/// @throws AudioDeviceCreationException Si ocurre algún error al establecer el 
		/// device por defecto.
		void FindDevices();


		/// @brief Nombre del dispositivo en uso actualmente.
		std::string currentDeviceName;

		/// @brief Nombre del dispositivo por defecto.
		std::string defaultDeviceName;
				
		/// @brief Todos los dispositivos disponibles.
		std::unordered_map<std::string, Device> devices;

	};

}
