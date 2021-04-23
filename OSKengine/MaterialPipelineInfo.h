#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialBinding.h"
#include "Renderpass.h"

#include <vector>
#include <string>

namespace OSK {

	/// <summary>
	/// Modo de renderizado de los polígonos: llenos o lineas.
	/// </summary>
	enum class PolygonMode {

		/// <summary>
		/// Se renderizan los polígonos enteros.
		/// </summary>
		FILL,

		/// <summary>
		/// Se renderizan los bordes.
		/// </summary>
		LINE

	};


	/// <summary>
	/// Modo de cull de los polígonos.
	/// Algunos polígonos pueden descartarse si dan la espalda a la cámara.
	/// </summary>
	enum class PolygonCullMode {

		/// <summary>
		/// Se descartan los polígonos que miran hacia la cámara.
		/// </summary>
		FRONT,

		/// <summary>
		/// Se descartan los polígonos que no miran hacia la cámara.
		/// </summary>
		BACK,

		/// <summary>
		/// No se descarta ningún polígono.
		/// </summary>
		NONE

	};

	/// <summary>
	/// Qué cara del polígono mira hacia delante.
	/// </summary>
	enum class PolygonFrontFaceType {

		/// <summary>
		/// Mira hacia delante si los vértices están en orden de las agujas del reloj.
		/// </summary>
		CLOCKWISE,

		/// <summary>
		/// /// Mira hacia delante si los vértices están en orden contrario de las agujas del reloj.
		/// </summary>
		COUNTERCLOCKWISE

	};

	/// <summary>
	/// Información sobre un push constant.
	/// Los push constants mandand información de cada instancia a la GPU.
	/// </summary>
	struct OSKAPI_CALL PushConstantInfo {

		/// <summary>
		/// Stage en el que se accede.
		/// </summary>
		MaterialBindingShaderStage ShaderStage;

		/// <summary>
		/// Tamaño de la estructura.
		/// </summary>
		size_t Size;

	};


	/// <summary>
	/// Información sobre un graphics pipeline.
	/// </summary>
	struct OSKAPI_CALL MaterialPipelineCreateInfo {

		/// <summary>
		/// Ruta del vertex shader (con extensión del archivo).
		/// </summary>
		std::string VertexPath;

		/// <summary>
		/// /// Ruta del fragment shader (con extensión del archivo).
		/// </summary>
		std::string FragmentPath;

		/// <summary>
		/// True si no se renderizarán polígonos que estén fuera del campo de visión de la cámrara.
		/// </summary>
		bool CullFaces = true;

		/// <summary>
		/// Modo de renderizado de los polígonos: llenos o lineas.
		/// </summary>
		PolygonMode PMode = PolygonMode::FILL;

		/// <summary>
		/// Modo de cull de los polígonos.
		/// Algunos polígonos pueden descartarse si dan la espalda a la cámara.
		/// </summary>
		PolygonCullMode CullMode = PolygonCullMode::FRONT;

		/// <summary>
		/// Qué cara del polígono mira hacia delante.
		/// </summary>
		PolygonFrontFaceType FrontFaceType = PolygonFrontFaceType::CLOCKWISE;

		/// <summary>
		/// True si usa funciones de profundidad / stencil.
		/// </summary>
		bool UseDepthStencil = false;

		/// <summary>
		/// Información de los push constants.
		/// </summary>
		std::vector<PushConstantInfo> PushConstants;

	};

	/// <summary>
	/// Devuelve el modo de polígonos en formato Vulkan.
	/// </summary>
	/// <param name="mode">Formato OSK.</param>
	/// <returns>Formato Vulkan.</returns>
	VkPolygonMode GetVkPolygonMode(PolygonMode mode);

	/// <summary>
	/// Devuelve el modo de cull en formato Vulkan.
	/// </summary>
	/// <param name="mode">Formato OSK.</param>
	/// <returns>Formato Vulkan.</returns>
	VkCullModeFlagBits GetVkCullMode(PolygonCullMode mode);

	/// <summary>
	/// Devuelve el modo de front face en formato Vulkan.
	/// </summary>
	/// <param name="mode">Formato OSK.</param>
	/// <returns>Formato Vulkan.</returns>
	VkFrontFace GetVkPolygonFrontFace(PolygonFrontFaceType mode);

}