#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Tipos de datos de entrada que puede tener un shader.
	/// </summary>
	enum class ShaderBindingType {

		/// <summary>
		/// Buffer con informaci�n arbitraria.
		/// </summary>
		UNIFORM_BUFFER,

		/// <summary>
		/// Textura 2D.
		/// </summary>
		TEXTURE,

		/// <summary>
		/// Textura c�bica.
		/// </summary>
		CUBEMAP

	};

	/// <summary>
	/// Shaders en los que se puede acceder a un elemento.
	/// 
	/// @note Flags: pueden acceder en varios stages.
	/// </summary>
	enum class ShaderStage {

		/// <summary>
		/// Shader de v�rtices.
		/// </summary>
		VERTEX = 1,

		/// <summary>
		/// Shader de fragmentos / p�xeles.
		/// </summary>
		FRAGMENT = 2
	};

}

OSK_FLAGS(OSK::GRAPHICS::ShaderStage);
