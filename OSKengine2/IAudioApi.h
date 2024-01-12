#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"

#include "IAudioDevice.h"
#include "HashMap.hpp"

#include "Vector3.hpp"

namespace OSK::ECS { class Transform3D; }


namespace OSK::AUDIO {

	class IAudioSource;


	class IAudioApi {

	public:

		virtual ~IAudioApi() = default;

		OSK_DEFINE_AS(IAudioApi);


		/// @brief Inicializa la API de bajo nivel y realiza una búsqueda
		/// de los dispositivos de salida disponibles.
		/// @throws AudioDeviceCreationException Si ocurre algún error al crear un device.
		/// @throws AudioDeviceCreationException Si ocurre algún error al establecer el 
		/// device por defecto.
		virtual void Initialize() = 0;

		/// @brief Establece la posición y orientación del jugador (listener).
		/// @param listenerTransform Transform del jugador.
		/// @see SetListenerPosition.
		/// @see SetListenerOrientation.
		void SetListener(const ECS::Transform3D& listenerTransform);

		/// @brief Establece la posición del jugador.
		/// @param position Posición del jugador en el mundo 3D.
		virtual void SetListenerPosition(const Vector3f& position) = 0;

		/// @brief Establece la orientación del jugador.
		/// @param forward Vector hacia delante del jugador.
		/// @param up Vector hacia arriba del jugador.
		virtual void SetListenerOrientation(const Vector3f& forward, const Vector3f& up) = 0;

		/// @brief Establece la velocidad a la que se mueve el jugador en el 
		/// mundo 3D.
		/// @param velocity Velocidad del jugador.
		virtual void SetListenerVelocity(const Vector3f& velocity) = 0;


		/// @return Devuelve una referencia al dispositivo de
		/// salida que se está usando en un momento dado.
		const IAudioDevice& GetCurrentDevice() const;

		/// @param name Nombre del dispositivo.
		/// @return Dispositivo de salida con el nombre @p name.
		/// @pre Debe haber un dispositivo cuyo nombre sea @p name.
		/// 
		/// @throws InvalidArgumentException si la precondición no se cumple.
		const IAudioDevice& GetDevice(std::string_view name) const;

		/// @brief Establece el dispositivo de salida a usar.
		/// @param name Nombre del dispositivo deseado.
		/// @pre Debe haber un dispositivo cuyo nombre sea @p name.
		/// 
		/// @throws InvalidArgumentException si la precondición no se cumple.
		void SetCurrentDevice(std::string_view name);

		/// @return Todos los dispositivos disponibles.
		const std::unordered_map<std::string, UniquePtr<IAudioDevice>, StringHasher, std::equal_to<>>& GetAllDevices() const;


		/// @return Nueva fuente de audio.
		virtual OwnedPtr<IAudioSource> CreateNewSource() const = 0;

	protected:

		/// @brief Busca todos los dispositivos de salida disponibles,
		/// y los almacena en la variable @p devices.
		/// @throws AudioDeviceCreationException Si ocurre algún error al crear un device.
		/// @throws AudioDeviceCreationException Si ocurre algún error al establecer el 
		/// device por defecto.
		virtual void RegisterAllDevices() = 0;

		void RegisterDevice(OwnedPtr<IAudioDevice> device);

		void SetDefaultDeviceName(const std::string& name);

	private:

		/// @brief Nombre del dispositivo en uso actualmente.
		std::string m_currentDeviceName;

		/// @brief Nombre del dispositivo por defecto.
		std::string m_defaultDeviceName;

		/// @brief Todos los dispositivos disponibles.
		std::unordered_map<std::string, UniquePtr<IAudioDevice>, StringHasher, std::equal_to<>> m_devices;

	};

}
