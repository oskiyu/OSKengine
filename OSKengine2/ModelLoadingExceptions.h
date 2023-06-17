#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepción que se lanza cuando no se consiguen encontrar las posiciones de los vértices
	/// de un modelo 3D
	/// Asegúrese de que el modelo 3D contiene información de posición de vértices.
	class NoVertexPositionsFoundException : public EngineException {

	public:

		explicit NoVertexPositionsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexPositionsFoundException: El archivo del modelo 3D no contiene posiciones."),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se consiguen encontrar las normales de los vértices
	/// de un modelo 3D
	/// Asegúrese de que el modelo 3D contiene información de normales de vértices.
	class NoVertexNormalsFoundException : public EngineException {

	public:

		explicit NoVertexNormalsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexNormalsFoundException: El archivo del modelo 3D no contiene normales."),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se consiguen encontrar las tangentes de los vértices
	/// de un modelo 3D
	/// Asegúrese de que el modelo 3D contiene información de tangentes de vértices.
	class NoVertexTangentsFoundException : public EngineException {

	public:

		explicit NoVertexTangentsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexTangentsFoundException: El archivo del modelo 3D no contiene tangentes."),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se consiguen encontrar las coordenadas de texturas de los vértices
	/// de un modelo 3D
	/// Asegúrese de que el modelo 3D contiene información de coordenadas de texturas de vértices.
	class NoVertexTexCoordsFoundException : public EngineException {

	public:

		explicit NoVertexTexCoordsFoundException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoVertexTexCoordsFoundException: El archivo del modelo 3D no contiene coordenadas de texturas."),
				location) { }

	};


	/// @brief Excepción que se lanza cuando los índices de un modelo 3D son de un tipo no soportado.
	class UnsupportedIndexTypeException : public EngineException {

	public:

		explicit UnsupportedIndexTypeException(
			const std::string& type,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedIndexTypeException: El modelo contiene índices del tipo {} no soportado.", type),
				location) { }

	};

	/// @brief Excepción que se lanza cuando los joints de un modelo 3D son de un tipo no soportado.
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

	/// @brief Excepción que se lanza cuando el formato de las imágenes de un modelo no están soportadas.
	class UnsupportedModelImageFormatException : public EngineException {

	public:

		explicit UnsupportedModelImageFormatException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedModelImageFormatException: El modelo contiene imágenes no soportadas."),
				location) { }

	};

	/// @brief Excepción que se lanza cuando el formato de los polígonos no es TRIÁNGULOS.
	class UnsupportedPolygonModeException : public EngineException {

	public:

		explicit UnsupportedPolygonModeException(
			std::string_view polygonMode,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnsupportedPolygonModeException: el modelo tiene polígonos {} en vez de TRIANGLES.", polygonMode),
				location) { }

	};

}
