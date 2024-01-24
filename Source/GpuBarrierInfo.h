#pragma once

#include "OSKmacros.h"
#include "EnumFlags.hpp"
#include "GpuImageUsage.h"

namespace OSK::GRAPHICS {

	constexpr USize32 ALL_MIP_LEVELS   = (~0u);
	constexpr USize32 ALL_IMAGE_LAYERS = (~0u);


	/// @brief Un command stage permite diferenciar los distintos
	/// tipos de comandos ejecutables por la GPU.
	/// Se usan en una barrera para sincronizar los comandos de distintos
	/// tipos.
	enum class GpuCommandStage {

		/// @brief Procesado de los vértices en el vertex shader.
		VERTEX_SHADER,

		/// @brief Procesado de los píxeles en el fragment/pixel shader.
		FRAGMENT_SHADER,

		/// @brief Ejecución del shader de control del teselado.
		TESSELATION_CONTROL,

		/// @brief Ejecución del shader de evaluación del teselado.
		TESSELATION_EVALUATION,

		/// @brief Shaders de trazado de rayos.
		RAYTRACING_SHADER,


		/// @brief Uso de la imagen como render target del pipeline gráfico.
		COLOR_ATTACHMENT_OUTPUT,

		/// @brief Uso como depth/stencil.
		DEPTH_STENCIL_START,

		/// @brief Uso como depth/stencil.
		DEPTH_STENCIL_END,


		/// @brief Transferencia / copia de datos.
		TRANSFER,


		/// @brief Shader de computación.
		COMPUTE_SHADER,

		/// @brief Construcción de estructuras de aceleración de trazado de rayos.
		RT_AS_BUILD,


		/// @brief Ningún comando será afectado.
		NONE,

		/// @brief Todos los comandos serán afectados.
		ALL

	};

	/// @brief Tipos de acceso de memoria que puede realizar la GPU.
	/// En un barrier, sirven para asegurarse de que los niveles
	/// de chaché sobn coherentes.
	enum class GpuAccessStage : uint32_t {

		
		/// @brief Lectura de cualquier tipo en shaders.
		/// Combinación de SAMPLED_READ, UNIFORM_BUFFER_READ, STORAGE_BUFFER_READ
		/// y STORAGE_IMAGE_READ.
		SHADER_READ = 1 << 0,

		/// @brief Lectura de una imagen sampled.
		SAMPLED_READ = 1 << 1,

		/// @brief Lectura de un uniform buffer.
		UNIFORM_BUFFER_READ = 1 << 2,

		/// @brief Lectura de un storage buffer.
		STORAGE_BUFFER_READ = 1 << 3,

		/// @brief Lectura de un storage image.
		STORAGE_IMAGE_READ = 1 << 4,


		/// @brief Escritrura de los datos en cualquier shader.
		SHADER_WRITE = 1 << 5,


		/// @brief Lectura de los datos de color durante shaders de rasterizado.
		COLOR_ATTACHMENT_READ = 1 << 6,

		/// @brief Escritura a un render target durante shaders de rasterizado
		COLOR_ATTACHMENT_WRITE = 1 << 7,


		/// @brief Lectura de depth/stencil.
		DEPTH_STENCIL_READ = 1 << 8,

		/// @brief Escritura de depth/stencil.
		DEPTH_STENCIL_WRITE = 1 << 9,


		/// @brief Lectura de los datos en una operación de transferencia / copia.
		TRANSFER_READ = 1 << 10,

		/// @brief Escritura de los datos en una operación de transferencia / copia.
		TRANSFER_WRITE = 1 << 11,


		/// @brief Lectura de cualquier tipo de la memoria.
		MEMORY_READ = 1 << 12,

		/// @brief Escritura de cualquier tipo de la memoria.
		MEMORY_WRITE = 1 << 13,


		/// @brief Lectura de la estructura de aceleración.
		RT_AS_READ = 1 << 14,

		/// @brief Escritura de la estructura de aceleración.
		RT_AS_WRITE = 1 << 15,


		/// @brief No representa ningún acceso a memoria.
		NONE = 0

	};


	/// <summary>
	/// Al establecer un gpu barrier, dividimos todos
	/// los comandos en la lista de comandos en dos grupos.
	/// 
	/// Al establecer el barrier, la GPU esperará hasta que se
	/// hayan completado todos los comandos en el stage sourceStage.
	/// 
	/// Todos los comandos enviados después del gpu barrier deben esperar
	/// a que finalicen todos los comandos en el stage destinationStage.
	/// </summary>
	struct GpuBarrierInfo {
		GpuBarrierInfo() = default;
		GpuBarrierInfo(GpuCommandStage stage, GpuAccessStage accessStage) : stage(stage), accessStage(accessStage) {}

		GpuCommandStage stage = GpuCommandStage::NONE;
		GpuAccessStage accessStage = GpuAccessStage::NONE;
	};
		
}

OSK_FLAGS(OSK::GRAPHICS::GpuAccessStage);
