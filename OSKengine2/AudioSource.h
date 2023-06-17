#pragma once

#include "AudioBuffer.h"
#include "Vector3.hpp"

namespace OSK::AUDIO {

	/// @brief Representa una fuente de sonido, capaz de reproducir
	/// audio.
	class OSKAPI_CALL Source {

	public:

		/// @brief Identificador de una fuente de audio.
		using Handle = ALCuint;

		/// @brief Identificador vacío / nulo.
		constexpr static Handle EMPTY_HANDLE = 0;

	public:

		/// @brief Estado en el que puede estar una fuente
		/// de audio.
		enum class State {

			/// @brief El audio se está reproduciendo.
			PLAYING,

			/// @brief El audio está pausado.
			PAUSED,

			/// @brief El audio está parado (en su posición inicial).
			STOPPED

		};

	public:

		/// @brief Vacío.
		Source();
		/// @brief Destruye la fuente nativa.
		~Source();

		OSK_DISABLE_COPY(Source)
		OSK_DEFAULT_MOVE_OPERATOR(Source)

		/// @brief Genera la fuente nativa.
		/// @throws AudioException Si ocurre algún error.
		void Init();

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


		/// @return Estado actual el source.
		State GetCurrentState() const;

		/// @brief Establece el estado del source,
		/// reproduciendo, pausando o parando el audio.
		/// @param state Nuevo estado.
		/// 
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		/// 
		/// @see Play().
		/// @see Pause().
		/// @see Stop().
		void SetCurrentState(State state);


		/// @brief Reproduce el audio del source.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		void Play();

		/// @brief Pausa el audio del source.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		void Pause();

		/// @brief Detiene el audio del source, devolviéndolo al estado inicial.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		void Stop();


		/// @brief Establece la posición del source.
		/// @param position Posición en el mundo 3D.
		void SetPosition(const Vector3f& position);

		/// @brief Establece la velocidad del source.
		/// @param position Velocidad en el mundo 3D.
		void SetVelocity(const Vector3f& velocity);


		/// @brief Establece el pitch del source.
		/// Por defecto 1.0.
		/// @param pitch Nuevo pitch.
		void SetPitch(float pitch);

		/// @brief Establece el gain ('volumen') del source.
		/// Por defecto 1.0.
		/// @param pitch Nuevo gain.
		/// @pre @p gain >  0.0.
		void SetGain(float gain);

		/// @brief Establece si el audio se reproducirá en bucle.
		/// @param looping True si estaará en bucle. False en caso contrario.
		void SetLooping(bool looping);

	private:

		/// @brief Pitch con el que se reproducirá el audio.
		float pitch = 1.0f;

		/// @brief Gain con el que se reproducirá el audio.
		float gain = 1.0f;

		/// @brief True si el audio se reproducirá en bucle.
		/// False en caso contrario.
		bool loop = false;


		/// @brief Handle de este source.
		Handle handle = EMPTY_HANDLE;

		/// @brief Handle del buffer que contiene los datos del audio.
		Buffer::Handle bufferHandle = Buffer::EMPTY_HANDLE;

	};

}
