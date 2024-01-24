#pragma once

#include "IAudioSource.h"

#include "AudioBuffer.h"
#include "Vector3.hpp"

namespace OSK::AUDIO {

	/// @brief Representa una fuente de sonido, capaz de reproducir
	/// audio.
	class OSKAPI_CALL AudioSourceAl : public IAudioSource {

	public:

		/// @brief Identificador de una fuente de audio.
		using Handle = ALCuint;

		/// @brief Identificador vacío / nulo.
		constexpr static Handle EMPTY_HANDLE = 0;


	public:

		/// @brief Destruye la fuente nativa.
		~AudioSourceAl() override;

		void Initialize() override;

		/// @brief Enlaza el source a un buffer.
		/// Necesario para poder reproducir audio.
		/// El audio reproducido será el almacenado
		/// en el buffer.
		/// @param buffer Buffer con el audio.
		/// 
		/// @pre @p buffer debe haber sido correctamente inicializado.
		/// @pre @p buffer debe haber sido correctamente asignado con los datos
		/// del audio.
		/// 
		/// @throws AudioException Si no se inicializó el Source con Init.
		/// @throws AudioException Si ocurre algún error al establecer el buffer.
		void SetBuffer(const Buffer& buffer);

		void SetPosition(const Vector3f& position) override;
		void SetVelocity(const Vector3f& velocity) override;

		void Play_Implementation() override;
		void Pause_Implementation() override;
		void Stop_Implementation() override;

		void SetPitch_Implementation(float pitch) override;
		void SetGain_Implementation(float gain) override;
		void SetLooping_Implementation(bool looping) override;

	private:

		/// @brief Handle de este source.
		Handle m_handle = EMPTY_HANDLE;

		/// @brief Handle del buffer que contiene los datos del audio.
		Buffer::Handle m_bufferHandle = Buffer::EMPTY_HANDLE;

	};

}
