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
	/// Modo de renderizado de los pol�gonos: llenos o lineas.
	/// </summary>
	enum class PolygonMode {

		/// <summary>
		/// Se renderizan los pol�gonos enteros.
		/// </summary>
		FILL,

		/// <summary>
		/// Se renderizan los bordes.
		/// </summary>
		LINE

	};


	/// <summary>
	/// Modo de cull de los pol�gonos.
	/// Algunos pol�gonos pueden descartarse si dan la espalda a la c�mara.
	/// </summary>
	enum class PolygonCullMode {

		/// <summary>
		/// Se descartan los pol�gonos que miran hacia la c�mara.
		/// </summary>
		FRONT,

		/// <summary>
		/// Se descartan los pol�gonos que no miran hacia la c�mara.
		/// </summary>
		BACK,

		/// <summary>
		/// No se descarta ning�n pol�gono.
		/// </summary>
		NONE

	};

	/// <summary>
	/// Qu� cara del pol�gono mira hacia delante.
	/// </summary>
	enum class PolygonFrontFaceType {

		/// <summary>
		/// Mira hacia delante si los v�rtices est�n en orden de las agujas del reloj.
		/// </summary>
		CLOCKWISE,

		/// <summary>
		/// /// Mira hacia delante si los v�rtices est�n en orden contrario de las agujas del reloj.
		/// </summary>
		COUNTERCLOCKWISE

	};

	/// <summary>
	/// Informaci�n sobre un push constant.
	/// Los push constants mandand informaci�n de cada instancia a la GPU.
	/// </summary>
	struct OSKAPI_CALL PushConstantInfo {

		/// <summary>
		/// Stage en el que se accede.
		/// </summary>
		MaterialBindingShaderStage shaderStage;

		/// <summary>
		/// Tama�o de la estructura.
		/// </summary>
		size_t size;

	};


	/// <summary>
	/// Informaci�n sobre un graphics pipeline.
	/// </summary>
	struct OSKAPI_CALL MaterialPipelineCreateInfo {

		/// <summary>
		/// Ruta del vertex shader (con extensi�n del archivo).
		/// </summary>
		std::string vertexPath;

		/// <summary>
		/// /// Ruta del fragment shader (con extensi�n del archivo).
		/// </summary>
		std::string fragmentPath;

		/// <summary>
		/// True si no se renderizar�n pol�gonos que est�n fuera del campo de visi�n de la c�mrara.
		/// </summary>
		bool cullFaces = true;

		/// <summary>
		/// Modo de renderizado de los pol�gonos: llenos o lineas.
		/// </summary>
		PolygonMode polygonMode = PolygonMode::FILL;

		/// <summary>
		/// Modo de cull de los pol�gonos.
		/// Algunos pol�gonos pueden descartarse si dan la espalda a la c�mara.
		/// </summary>
		PolygonCullMode cullMode = PolygonCullMode::FRONT;

		/// <summary>
		/// Qu� cara del pol�gono mira hacia delante.
		/// </summary>
		PolygonFrontFaceType frontFaceType = PolygonFrontFaceType::CLOCKWISE;

		/// <summary>
		/// True si usa funciones de profundidad / stencil.
		/// </summary>
		bool useDepthStencil = false;

		/// <summary>
		/// Informaci�n de los push constants.
		/// </summary>
		std::vector<PushConstantInfo> pushConstants;

	};

	/// <summary>
	/// Devuelve el modo de pol�gonos en formato Vulkan.
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