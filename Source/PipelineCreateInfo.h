#pragma once

#include <string>

#include "DynamicArray.hpp"
#include "Format.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Modo de renderizado de los pol�gonos: llenos o lineas.
	/// </summary>
	enum class PolygonMode {

		/// <summary>
		/// Se renderizan los tri�ngulos enteros.
		/// </summary>
		TRIANGLE_FILL,

		/// <summary>
		/// Se renderizan los bordes de los tri�ngulos.
		/// </summary>
		TRIANGLE_WIDEFRAME,

		/// @brief Se renderizan l�neas rectas.
		LINE,

		/// @brief Se renderizan puntos.
		POINT

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
	/// Establece la configuraci�n de testeo de profundidad.
	/// Activarlo permite que los objetos en la imagen final sean los que m�s
	/// cerca est�n de la c�mara.
	/// </summary>
	enum class DepthTestingType {

		/// <summary>
		/// Ninguna operaci�n.
		/// </summary>
		NONE,

		/// <summary>
		/// Lee la informaci�n para comprobar que el objeto debe ser renderizado,
		/// pero no actualiza la informaci�n.
		/// </summary>
		READ,

		/// <summary>
		/// Comportamiento normal.
		/// </summary>
		READ_WRITE

	};

	/// <summary>
	/// Contiene la informaci�n necesaria para configurar un nuevo graphics pipeline.
	/// </summary>
	struct PipelineCreateInfo {

		/// <summary>
		/// Direcci�n en la que se encuentra el shader de v�rtices.
		/// 
		/// @note Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		/// </summary>
		std::string vertexPath;

		/// <summary>
		/// Direcci�n en la que se encuentra el shader de fragmentos o p�xeles.
		/// 
		/// @note Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		/// </summary>
		std::string fragmentPath;

		/// <summary>
		/// Direcci�n en la que se encuentra el shader de control de la teselaci�n.
		/// Este shader es el encargado de determinar qu� grado de teselaci�n se 
		/// realizar� en una primitiva.
		/// </summary>
		/// 
		/// @note Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		std::string tesselationControlPath = "";

		/// <summary>
		/// Direcci�n en la que se encuentra el shader de evaluaci�n de la teselaci�n.
		/// Este shader es el encargado de generar los v�rtices teselados.
		/// </summary>
		/// 
		/// @note Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		std::string tesselationEvaluationPath = "";

		std::string rtRaygenShaderPath = "";
		std::string rtClosestHitShaderPath = "";
		std::string rtMissShaderPath = "";

		std::string computeShaderPath = "";

		/// <summary>
		/// N�mero de v�rtices que tendr� cada patch del teselado.
		/// </summary>
		USize32 tesselationPatchControlPoints = 3;

		/// <summary>
		/// Tipo de pol�gono (l�nea, tri�ngulo...).
		/// </summary>
		PolygonMode polygonMode;

		/// <summary>
		/// Establece si no se renderizar�n los pol�gonos que est�n de espaldas
		/// a la c�mara.
		/// </summary>
		PolygonCullMode cullMode = PolygonCullMode::FRONT;

		/// <summary>
		/// Establece c�mo saber qu� cara es la delantera del tri�ngulo.
		/// Para poder usar correctamente CullMode.
		/// 
		/// Debe elegirse si la cara de frente tiene los v�rtices definidos en sentido
		/// horario o antihorario.
		/// </summary>
		PolygonFrontFaceType frontFaceType;

		/// <summary>
		/// Establece la configuraci�n de testeo de profundidad.
		/// Activarlo permite que los objetos en la imagen final sean los que m�s
		/// cerca est�n de la c�mara.
		/// </summary>
		DepthTestingType depthTestingType = DepthTestingType::READ_WRITE;

		/// <summary>
		/// True si los shaders hlsl est�n precompilados.
		/// False si se deben compilar en tiempo de creaci�n del
		/// pipeline.
		/// </summary>
		bool precompiledHlslShaders = true;

		bool isRaytracing = false;

		/// <summary>
		/// Formato de la imagen sobre la que se renderizar�.
		/// </summary>
		DynamicArray<Format> formats;

		/// @brief Formato de la imagen de profundidad.
		Format depthFormat;

		bool useAlphaBlending = true;

		bool usesUnspecifiedSizedArrays = false;
		USize32 maxUnspecifiedSizedArraysSize = 0;

	};

}
