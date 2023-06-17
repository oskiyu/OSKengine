#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepci�n que se lanza cuando no se consiguen encontrar las posiciones de los v�rtices
	/// de un modelo 3D
	/// Aseg�rese de que el modelo 3D contiene informaci�n de posici�n de v�rtices.
	class NoVertexPositionsFoundException : public EngineException {

	public:

		explicit NoVertexPositionsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexPositionsFoundException: El archivo del modelo 3D no contiene posiciones."),
				location) { }

	};

	/// @brief Excepci�n que se lanza cuando no se consiguen encontrar las normales de los v�rtices
	/// de un modelo 3D
	/// Aseg�rese de que el modelo 3D contiene informaci�n de normales de v�rtices.
	class NoVertexNormalsFoundException : public EngineException {

	public:

		explicit NoVertexNormalsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexNormalsFoundException: El archivo del modelo 3D no contiene normales."),
				location) { }

	};

	/// @brief Excepci�n que se lanza cuando no se consiguen encontrar las tangentes de los v�rtices
	/// de un modelo 3D
	/// Aseg�rese de que el modelo 3D contiene informaci�n de tangentes de v�rtices.
	class NoVertexTangentsFoundException : public EngineException {

	public:

		explicit NoVertexTangentsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexTangentsFoundException: El archivo del modelo 3D no contiene tangentes."),
				location) { }

	};

	/// @brief Excepci�n que se lanza cuando no se consiguen encontrar las coordenadas de texturas de los v�rtices
	/// de un modelo 3D
	/// Aseg�rese de que el modelo 3D contiene informaci�n de coordenadas de texturas de v�rtices.
	class NoVertexTexCoordsFoundException : public EngineException {

	public:

		explicit NoVertexTexCoordsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexTexCoordsFoundException: El archivo del modelo 3D no contiene coordenadas de texturas."),
				location) { }

	};


	/// @brief Excepci�n que se lanza cuando los �ndices de un modelo 3D son de un tipo no soportado.
	class UnsupportedIndexTypeException : public EngineException {

	public:

		explicit UnsupportedIndexTypeException(
			const std::string& type,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedIndexTypeException: El modelo contiene �ndices del tipo {} no soportado.", type),
				location) { }

	};

	/// @brief Excepci�n que se lanza cuando los joints de un modelo 3D son de un tipo no soportado.
	class UnsupportedJointTypeException : public EngineException {

	public:

		explicit UnsupportedJointTypeException(
			const std::string& type,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedJointTypeException: El modelo contiene joints del tipo {} no soportado.", type),
				location) { }

	};

	/// @brief Excepci�n que se lanza cuando el formato de las im�genes de un modelo no est�n soportadas.
	class UnsupportedModelImageFormatException : public EngineException {

	public:

		explicit UnsupportedModelImageFormatException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedModelImageFormatException: El modelo contiene im�genes no soportadas."),
				location) { }

	};

	/// @brief Excepci�n que se lanza cuando el formato de los pol�gonos no es TRI�NGULOS.
	class UnsupportedPolygonModeException : public EngineException {

	public:

		explicit UnsupportedPolygonModeException(
			std::string_view polygonMode,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedPolygonModeException: el modelo tiene pol�gonos {} en vez de TRIANGLES.", polygonMode),
				location) { }

	};

}
