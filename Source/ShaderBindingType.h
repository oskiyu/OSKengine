#pragma once

#include "EnumFlags.hpp"
#include "ToString.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Tipos de datos de entrada que puede tener un shader.
	/// </summary>
	enum class ShaderBindingType {

		/// <summary>
		/// Buffer read-only con información arbitraria.
		/// </summary>
		UNIFORM_BUFFER,

		/// <summary>
		/// Textura 2D.
		/// </summary>
		TEXTURE,

		/// <summary>
		/// Imagen array.
		/// </summary>
		TEXTURE_ARRAY,

		/// <summary>
		/// Textura cúbica.
		/// </summary>
		CUBEMAP,

		/// <summary>
		/// Buffer read/write, que puede contener arrays con tamaño variable.
		/// </summary>
		STORAGE_BUFFER,

		/// <summary>
		/// Estructura de aceleración espacial con la geometría
		/// para el trazado de rayos.
		/// </summary>
		RT_ACCELERATION_STRUCTURE,

		/// <summary>
		/// Imagen read/write, que puede usarse en shaders de computación.
		/// </summary>
		STORAGE_IMAGE,

		/// <summary>
		/// Imagen en la que se renderizará el resultado del
		/// trazado de rayos.
		/// </summary>
		RT_TARGET_IMAGE = STORAGE_IMAGE

	};

	/// <summary>
	/// Shaders en los que se puede acceder a un elemento.
	/// 
	/// @note Flags: pueden acceder en varios stages.
	/// </summary>
	enum class ShaderStage {

		/// <summary>
		/// Shader de vértices.
		/// </summary>
		VERTEX = 1,

		/// <summary>
		/// Shader de fragmentos / píxeles.
		/// </summary>
		FRAGMENT = 2,

		/// <summary>
		/// Shaders de control de teselado.
		/// </summary>
		TESSELATION_CONTROL = 4,

		/// <summary>
		/// Shaders de evaluación de teselado.
		/// </summary>
		TESSELATION_EVALUATION = 8,

		/// <summary>
		/// Shader que genera los rayos.
		/// </summary>
		RT_RAYGEN = 16,

		/// <summary>
		/// Shader que se ejecuta al final del trazado
		/// de rayos, con el triángulo más cercano con el
		/// que haya colisionado.
		/// </summary>
		RT_CLOSEST_HIT = 32,

		/// <summary>
		/// Shader que se ejecuta cuando el rayo no ha dado
		/// con ningún triángulo.
		/// </summary>
		RT_MISS = 64,

		/// <summary>
		/// Shader para computación genérica.
		/// </summary>
		COMPUTE = 128,

		/// @brief Shader de amplificación / task shaders.
		MESH_AMPLIFICATION = 256,

		/// @brief Shader de meshes.
		MESH = 512,

		/// <summary>
		/// Todos los shaders de teselado.
		/// </summary>
		TESSELATION_ALL = TESSELATION_CONTROL | TESSELATION_EVALUATION,

		RT_ALL = RT_RAYGEN | RT_CLOSEST_HIT |RT_MISS

	};

}

OSK_FLAGS(OSK::GRAPHICS::ShaderStage);

template <> std::string OSK::ToString<OSK::GRAPHICS::ShaderBindingType>(const OSK::GRAPHICS::ShaderBindingType& type);
template <> std::string OSK::ToString<OSK::GRAPHICS::ShaderStage>(const OSK::GRAPHICS::ShaderStage& stage);