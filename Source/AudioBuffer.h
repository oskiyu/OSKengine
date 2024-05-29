#pragma once

#include "ApiCall.h"
#include "DefineConstructors.h"

#include "NumericTypes.h"
#include "AudioFormat.h"

#include <AL/alc.h>

namespace OSK::AUDIO {

	/// @brief Buffer que se usa para almacenar total o parcialmente un
	/// archivo de audio reproducible.
	class OSKAPI_CALL Buffer {

	public:

		/// @brief Identificador de un buffer.
		using Handle = ALCuint;
		
		/// @brief Constructor vacío.
		Buffer() = default;
		/// @brief Destruye el buffer (si ha sido inicializado).
		~Buffer();

		OSK_DISABLE_COPY(Buffer);

		/// @brief Inicicaliza el buffer.
		/// @throws AudioException Si ocurre algún error al generar el buffer.
		void Init();

		/// @brief Establece el contenido del buffer.
		/// @param data Datos de sonido.
		/// @param size Tamaño de los datos (en bytes).
		/// @param format Formato del audio.
		/// @param samplerate Tasa de refresco del audio.
		/// 
		/// @pre El buffer debe haber sido inicializado con Init.
		/// 
		/// @throws AudioException Si ocurre algún error al rellenar el buffer.
		void Fill(const void* data, USize64 size, Format format, USize32 samplerate);

		/// @return Identificador del buffer.
		Handle GetHandle() const;

	public:

		/// @brief Identificador vacío / nulo.
		constexpr static Handle EMPTY_HANDLE = 0;

	private:

		/// @brief Identificador nativo del buffer.
		Handle handle = 0;

	};

}
