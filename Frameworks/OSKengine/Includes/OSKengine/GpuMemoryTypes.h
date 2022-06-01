#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Define qu� dispositivo tiene acceso a la memoria.
	/// 
	/// Puede definir una memoria exclusiva para la GPU (mayor
	/// rendimiento) o una memoria accesible tanto desde la CPU
	/// como la GPU.
	/// </summary>
	enum class GpuSharedMemoryType {

		/// <summary>
		/// Compartido.
		/// �til para recursos que van a ser sobreescritos a menudo, o
		/// recursos de un solo uso a los que vamos a escribir informaci�n
		/// directamente, como buffers intermedios.
		/// </summary>
		GPU_AND_CPU,

		/// <summary>
		/// Exclusivo para la GPU.
		/// Mejor rendimiento.
		/// @warning No se puede escribir directamente desde la CPU: se debe crear un buffer
		/// intermedio al que tengamos acceso (GpuSharedMemoryType::GPU_AND_CPU), y despu�s 
		/// copiar sus contenidos usando la cola de comandos.
		/// </summary>
		GPU_ONLY

	};

	/// <summary>
	/// Tipo de uso que se le va a dar a la memoria.
	/// Imagen o buffer.
	/// </summary>
	enum class GpuMemoryUsage {
		BUFFER,
		IMAGE
	};

	/// <summary>
	/// Uso que se le va a dar a un buffer.
	/// Pueden combinarse varios de estos usos.
	/// </summary>
	enum class GpuBufferUsage {

		/// <summary>
		/// Buffer que ser� le�do desde los shaders.
		/// 
		/// @pre Debe tener GpuSharedMemoryType::GPU_AND_CPU.
		/// </summary>
		UNIFORM_BUFFER = 1,

		/// <summary>
		/// Buffer que contendr� los v�rtices de modelos 3D.
		/// </summary>
		VERTEX_BUFFER = 2,

		/// <summary>
		/// Buffer que contendr� los �ndices de modelos 3D.
		/// </summary>
		INDEX_BUFFER = 4,

		/// <summary>
		/// Se usar� para almacenar datos que despu�s ser�n copiados
		/// a otro buffer.
		/// </summary>
		TRANSFER_SOURCE = 8,

		/// <summary>
		/// Se copiar�n datos de otro buffer a este.
		/// </summary>
		TRANSFER_DESTINATION = 16

	};

}

OSK_FLAGS(OSK::GRAPHICS::GpuBufferUsage);
