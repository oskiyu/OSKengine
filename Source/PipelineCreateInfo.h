#pragma once

#include <string>

#include "DynamicArray.hpp"
#include "Format.h"

namespace OSK::GRAPHICS {

	/// @brief Modo de renderizado de los polígonos.
	enum class PolygonMode {

		/// @brief Se renderizan los triángulos enteros.
		TRIANGLE_FILL,

		/// @brief Se renderizan los bordes de los triángulos.
		TRIANGLE_WIDEFRAME,

		/// @brief Se renderizan líneas rectas.
		LINE,

		/// @brief Se renderizan puntos.
		POINT

	};


	/// @brief Modo de cull de los polígonos.
	/// Algunos polígonos pueden descartarse si dan la espalda a la cámara.
	enum class PolygonCullMode {

		/// @brief Se descartan los polígonos que miran hacia la cámara.
		FRONT,

		/// @brief Se descartan los polígonos que no miran hacia la cámara.
		BACK,

		/// @brief No se descarta ningún polígono.
		NONE

	};


	/// @brief Qué cara del polígono mira hacia delante.
	enum class PolygonFrontFaceType {

		/// @brief Mira hacia delante si los vértices están en orden de las agujas del reloj.
		CLOCKWISE,

		/// @brief Mira hacia delante si los vértices están en orden contrario de las agujas del reloj.
		COUNTERCLOCKWISE

	};


	/// @brief Establece la configuración de testeo de profundidad.
	/// Activarlo permite que los objetos en la imagen final sean los que más
	/// cerca están de la cámara.
	enum class DepthTestingType {

		/// @brief Ninguna operación.
		NONE,

		/// @brief Lee la información para comprobar que el objeto debe ser renderizado,
		/// pero no actualiza la información.
		READ,

		/// @brief Comportamiento normal.
		READ_WRITE

	};


	/// @brief Contiene la información necesaria para configurar un nuevo pipeline.
	struct PipelineCreateInfo {

		/// @brief Dirección en la que se encuentra el shader de vértices.
		std::string vertexPath;

		/// @brief Dirección en la que se encuentra el shader de fragmentos o píxeles.
		std::string fragmentPath;
				
		/// @brief Dirección en la que se encuentra el shader de control de la teselación.
		std::string tesselationControlPath = "";

		/// @brief Dirección en la que se encuentra el shader de evaluación de la teselación.
		std::string tesselationEvaluationPath = "";


		/// @brief Dirección en la que se encuentra el shader de generación de rayos.
		std::string rtRaygenShaderPath = "";

		/// @brief Dirección en la que se encuentra el shader de hit de rayos.
		std::string rtClosestHitShaderPath = "";

		/// @brief Dirección en la que se encuentra el shader de miss de rayos.
		std::string rtMissShaderPath = "";


		/// @brief Dirección en la que se encuentra el shader de ampliación de meshes.
		std::string meshAmplificationShaderPath = "";

		/// @brief Dirección en la que se encuentra el shader de meshes.
		std::string meshShaderPath = "";


		/// @brief Dirección en la que se encuentra el shader de computación.
		std::string computeShaderPath = "";


		/// @brief Número de vértices que tendrá cada patch del teselado.
		USize32 tesselationPatchControlPoints = 3;


		PolygonMode polygonMode;
		PolygonCullMode cullMode = PolygonCullMode::FRONT;
		PolygonFrontFaceType frontFaceType;

		DepthTestingType depthTestingType = DepthTestingType::READ_WRITE;


		/// @brief True si los shaders hlsl están precompilados.
		/// False si se deben compilar en tiempo de creación del
		/// pipeline.
		bool precompiledHlslShaders = true;

		/// <summary>
		/// Formato de la imagen sobre la que se renderizará.
		/// </summary>
		DynamicArray<Format> formats;

		/// @brief Formato de la imagen de profundidad.
		Format depthFormat;

		bool useAlphaBlending = true;

		bool usesUnspecifiedSizedArrays = false;
		USize32 maxUnspecifiedSizedArraysSize = 0;

	};

}
