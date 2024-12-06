#pragma once

#include <string>

#include "DynamicArray.hpp"
#include "Format.h"

namespace OSK::GRAPHICS {

	/// @brief Modo de renderizado de los pol�gonos.
	enum class PolygonMode {

		/// @brief Se renderizan los tri�ngulos enteros.
		TRIANGLE_FILL,

		/// @brief Se renderizan los bordes de los tri�ngulos.
		TRIANGLE_WIDEFRAME,

		/// @brief Se renderizan l�neas rectas.
		LINE,

		/// @brief Se renderizan puntos.
		POINT

	};


	/// @brief Modo de cull de los pol�gonos.
	/// Algunos pol�gonos pueden descartarse si dan la espalda a la c�mara.
	enum class PolygonCullMode {

		/// @brief Se descartan los pol�gonos que miran hacia la c�mara.
		FRONT,

		/// @brief Se descartan los pol�gonos que no miran hacia la c�mara.
		BACK,

		/// @brief No se descarta ning�n pol�gono.
		NONE

	};


	/// @brief Qu� cara del pol�gono mira hacia delante.
	enum class PolygonFrontFaceType {

		/// @brief Mira hacia delante si los v�rtices est�n en orden de las agujas del reloj.
		CLOCKWISE,

		/// @brief Mira hacia delante si los v�rtices est�n en orden contrario de las agujas del reloj.
		COUNTERCLOCKWISE

	};


	/// @brief Establece la configuraci�n de testeo de profundidad.
	/// Activarlo permite que los objetos en la imagen final sean los que m�s
	/// cerca est�n de la c�mara.
	enum class DepthTestingType {

		/// @brief Ninguna operaci�n.
		NONE,

		/// @brief Lee la informaci�n para comprobar que el objeto debe ser renderizado,
		/// pero no actualiza la informaci�n.
		READ,

		/// @brief Comportamiento normal.
		READ_WRITE

	};


	/// @brief Contiene la informaci�n necesaria para configurar un nuevo pipeline.
	struct PipelineCreateInfo {

		/// @brief Direcci�n en la que se encuentra el shader de v�rtices.
		std::string vertexPath;

		/// @brief Direcci�n en la que se encuentra el shader de fragmentos o p�xeles.
		std::string fragmentPath;
				
		/// @brief Direcci�n en la que se encuentra el shader de control de la teselaci�n.
		std::string tesselationControlPath = "";

		/// @brief Direcci�n en la que se encuentra el shader de evaluaci�n de la teselaci�n.
		std::string tesselationEvaluationPath = "";


		/// @brief Direcci�n en la que se encuentra el shader de generaci�n de rayos.
		std::string rtRaygenShaderPath = "";

		/// @brief Direcci�n en la que se encuentra el shader de hit de rayos.
		std::string rtClosestHitShaderPath = "";

		/// @brief Direcci�n en la que se encuentra el shader de miss de rayos.
		std::string rtMissShaderPath = "";


		/// @brief Direcci�n en la que se encuentra el shader de ampliaci�n de meshes.
		std::string meshAmplificationShaderPath = "";

		/// @brief Direcci�n en la que se encuentra el shader de meshes.
		std::string meshShaderPath = "";


		/// @brief Direcci�n en la que se encuentra el shader de computaci�n.
		std::string computeShaderPath = "";


		/// @brief N�mero de v�rtices que tendr� cada patch del teselado.
		USize32 tesselationPatchControlPoints = 3;


		PolygonMode polygonMode;
		PolygonCullMode cullMode = PolygonCullMode::FRONT;
		PolygonFrontFaceType frontFaceType;

		DepthTestingType depthTestingType = DepthTestingType::READ_WRITE;


		/// @brief True si los shaders hlsl est�n precompilados.
		/// False si se deben compilar en tiempo de creaci�n del
		/// pipeline.
		bool precompiledHlslShaders = true;

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
