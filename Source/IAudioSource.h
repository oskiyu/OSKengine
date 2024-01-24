#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"

namespace OSK::AUDIO {

	/// @brief Estado en el que puede estar una fuente
	/// de audio.
	enum class AudioState {

		/// @brief El audio se est� reproduciendo.
		PLAYING,

		/// @brief El audio est� pausado.
		PAUSED,

		/// @brief El audio est� parado (en su posici�n inicial).
		STOPPED,

		UNKNOWN

	};


	class OSKAPI_CALL IAudioSource {

	public:
		
		virtual ~IAudioSource() = default;

		OSK_DEFINE_AS(IAudioSource);


		virtual void Initialize();

		// --- Configuraci�n --- //

		/// @brief Establece la posici�n del source.
		/// @param position Posici�n en el mundo 3D.
		virtual void SetPosition(const Vector3f& position) = 0;

		/// @brief Establece la velocidad del source.
		/// @param position Velocidad en el mundo 3D.
		virtual void SetVelocity(const Vector3f& velocity) = 0;


		/// @brief Establece el pitch del source.
		/// Por defecto 1.0.
		/// @param pitch Nuevo pitch.
		void SetPitch(float pitch);

		/// @brief Establece el gain ('volumen') del source.
		/// Por defecto 1.0.
		/// @param pitch Nuevo gain.
		/// @pre @p gain >  0.0.
		void SetGain(float gain);

		/// @brief Establece si el audio se reproducir� en bucle.
		/// @param looping True si estaar� en bucle. False en caso contrario.
		void SetLooping(bool looping);


		// --- Estados --- //

		/// @brief Establece el estado del source,
		/// reproduciendo, pausando o parando el audio.
		/// @param state Nuevo estado.
		/// 
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		/// 
		/// @see Play().
		/// @see Pause().
		/// @see Stop().
		void SetCurrentState(AudioState state);

		/// @return Estado actual el source.
		AudioState GetCurrentState() const;


		/// @brief Reproduce el audio del source.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		void Play();

		/// @brief Pausa el audio del source.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		void Pause();

		/// @brief Detiene el audio del source, devolvi�ndolo al estado inicial.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		void Stop();
		
	protected:

		/// @brief Reproduce el audio del source.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		virtual void Play_Implementation() = 0;

		/// @brief Pausa el audio del source.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		virtual void Pause_Implementation() = 0;

		/// @brief Detiene el audio del source, devolvi�ndolo al estado inicial.
		/// @pre Se debe haber establecido un buffer con SetBuffer.
		virtual void Stop_Implementation() = 0;


		/// @brief Establece el pitch del source.
		/// Por defecto 1.0.
		/// @param pitch Nuevo pitch.
		virtual void SetPitch_Implementation(float pitch) = 0;

		/// @brief Establece el gain ('volumen') del source.
		/// Por defecto 1.0.
		/// @param pitch Nuevo gain.
		/// @pre @p gain >  0.0.
		virtual void SetGain_Implementation(float gain) = 0;

		/// @brief Establece si el audio se reproducir� en bucle.
		/// @param looping True si estaar� en bucle. False en caso contrario.
		virtual void SetLooping_Implementation(bool looping) = 0;

	private:

		/// @brief Pitch con el que se reproducir� el audio.
		float m_pitch = 1.0f;

		/// @brief Gain con el que se reproducir� el audio.
		float m_gain = 1.0f;

		/// @brief True si el audio se reproducir� en bucle.
		/// False en caso contrario.
		bool m_loop = false;

		/// @brief Estado actual de la fuente.
		AudioState m_state = AudioState::STOPPED;

	};

}
