#pragma once

#include "MaterialLayoutBinding.h"
#include <string>
#include "HashMap.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un MaterialLayoutSlot agrupa varios MaterialLayoutBindings: 
	/// son una manera de unir bindings que se usen en una frecuencia similar.
	/// </summary>
	struct MaterialLayoutSlot {

		/// <summary>
		/// Nombre del slot. 
		/// Debe ser único en el layout.
		/// </summary>
		std::string name;

		/// <summary>
		/// Todos los bindings que componene este slot.
		/// </summary>
		HashMap<std::string, MaterialLayoutBinding> bindings;

		/// <summary>
		/// Índice del descriptor set del shader .glsl (Vulkan/OpenGL) al que se asocia este slot.
		/// </summary>
		UIndex32 glslSetIndex;

		/// <summary>
		/// Stage del shader desde el que se accede al slot (VERTEX/FRAGMENT).
		/// </summary>
		ShaderStage stage;

	};

}
