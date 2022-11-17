#pragma once

#include "OSKmacros.h"
#include "EnumFlags.hpp"
#include "GpuImageUsage.h"

namespace OSK::GRAPHICS {

	constexpr TSize ALL_MIP_LEVELS = (~0);
	constexpr TSize ALL_IMAGE_LAYERS = (~0);



	enum class GpuBarrierStage {

		/// <summary>
		/// Procesado de los v�rtices en el vertex shader.
		/// </summary>
		VERTEX_SHADER,

		/// <summary>
		/// Procesado de los p�xeles en el fragment/pixel shader.
		/// </summary>
		FRAGMENT_SHADER,

		/// <summary>
		/// Ejecuci�n del shader de control del teselado.
		/// </summary>
		TESSELATION_CONTROL,

		/// <summary>
		/// Ejecuci�n del shader de evaluaci�n del teselado.
		/// </summary>
		TESSELATION_EVALUATION,

		/// <summary>
		/// Shaders de trazado de rayos.
		/// </summary>
		RAYTRACING_SHADER,

		/// <summary>
		/// Uso de la imagen como render target.
		/// </summary>
		COLOR_ATTACHMENT_OUTPUT,

		/// <summary>
		/// Uso como depth/stencil.
		/// </summary>
		DEPTH_STENCIL_START,

		/// <summary>
		/// Uso como depth/stencil.
		/// </summary>
		DEPTH_STENCIL_END,

		/// <summary>
		/// Transferencia de datos.
		/// </summary>
		TRANSFER,

		/// <summary>
		/// Shader de computaci�n.
		/// </summary>
		COMPUTE_SHADER,

		/// <summary>
		/// Construcci�n de estructuras de aceleraci�n.
		/// </summary>
		RT_AS_BUILD,

		/// <summary>
		/// @todo Documentaci�n.
		/// </summary>
		DEFAULT

	};

	/// <summary>
	/// Operaci�n en concreto que queremos esperar/bloquear.
	/// </summary>
	enum class GpuBarrierAccessStage {

		/// <summary>
		/// Lectura de los datos en cualquier shader.
		/// </summary>
		SHADER_READ = 1,

		/// <summary>
		/// Escritrura de los datos en cualquier shader.
		/// </summary>
		SHADER_WRITE = 2,

		/// <summary>
		/// Lectura de los datos de color de la imagen.
		/// </summary>
		COLOR_ATTACHMENT_READ = 4,

		/// <summary>
		/// Escritura a un render target.
		/// </summary>
		COLOR_ATTACHMENT_WRITE = 8,

		/// <summary>
		/// Lectura de depth/stencil.
		/// </summary>
		DEPTH_STENCIL_READ = 16,

		/// <summary>
		/// Escritura de depth/stencil.
		/// </summary>
		DEPTH_STENCIL_WRITE = 32,

		/// <summary>
		/// Lectura de los datos en una operaci�n de transferencia.
		/// </summary>
		TRANSFER_READ = 64,

		/// <summary>
		/// Escritura de los datos en una operaci�n de transferencia.
		/// </summary>
		TRANSFER_WRITE = 128,

		/// <summary>
		/// Lectura de la memoria.
		/// </summary>
		MEMORY_READ = 256,

		/// <summary>
		/// Escritura de la memoria.
		/// </summary>
		MEMORY_WRITE = 512,

		/// <summary>
		/// Lectura de la estructura de aceleraci�n.
		/// </summary>
		RT_AS_READ = 1024,

		/// <summary>
		/// Escritura de la estructura de aceleraci�n.
		/// </summary>
		RT_AS_WRITE = 2048,

		DEFAULT = 4096
	};


	/// <summary>
	/// Al establecer un gpu barrier, dividimos todos
	/// los comandos en la lista de comandos en dos grupos.
	/// 
	/// Al establecer el barrier, la GPU esperar� hasta que se
	/// hayan completado todos los comandos en el stage sourceStage.
	/// 
	/// Todos los comandos enviados despu�s del gpu barrier deben esperar
	/// a que finalicen todos los comandos en el stage destinationStage.
	/// </summary>
	struct GpuBarrierInfo {
		GpuBarrierInfo() {}
		GpuBarrierInfo(GpuBarrierStage stage, GpuBarrierAccessStage accessStage) : stage(stage), accessStage(accessStage) {}

		GpuBarrierStage stage = GpuBarrierStage::DEFAULT;
		GpuBarrierAccessStage accessStage = GpuBarrierAccessStage::DEFAULT;
	};


	/// <summary>
	/// Al establecer un gpu barrier, dividimos todos
	/// los comandos en la lista de comandos en dos grupos.
	/// 
	/// Al establecer el barrier, la GPU esperar� hasta que se
	/// hayan completado todos los comandos en el stage sourceStage.
	/// 
	/// Todos los comandos enviados despu�s del gpu barrier deben esperar
	/// a que finalicen todos los comandos en el stage destinationStage.
	/// </summary>
	struct GpuImageBarrierInfo {

		/// @brief �ndice de la primera capa que ser� afectada por el barrier.
		/// @pre Si la imagen NO es array, debe ser 0.
		TSize baseLayer = 0;
		
		/// @brief N�mero de capas del array afectadas por el barrier.
		TSize numLayers = ALL_IMAGE_LAYERS;

		/// @brief Nivel m�s bajo de mip que ser� afectado por el barrier.
		TSize baseMipLevel = 0;
		/// @brief N�mero de niveles de mip que ser�n afectados por el barrier.
		TSize numMipLevels = ALL_MIP_LEVELS;


		/// @brief Canal(es) afectados por el barrier.
		SampledChannel channel = SampledChannel::COLOR;

	};

}

OSK_FLAGS(OSK::GRAPHICS::GpuBarrierAccessStage);
