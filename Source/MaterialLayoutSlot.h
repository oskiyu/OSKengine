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
		/// Debe ser �nico en el layout.
		/// </summary>
		std::string name;

		/// <summary>
		/// Todos los bindings que componene este slot.
		/// </summary>
		std::unordered_map<std::string, MaterialLayoutBinding, StringHasher, std::equal_to<>> bindings;

		/// <summary>
		/// �ndice del descriptor set del shader .glsl (Vulkan/OpenGL) al que se asocia este slot.
		/// </summary>
		UIndex32 glslSetIndex = 0;

		/// <summary>
		/// Stage del shader desde el que se accede al slot (VERTEX/FRAGMENT).
		/// </summary>
		ShaderStage stage{};

	};

}

template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayoutSlot>(const OSK::GRAPHICS::MaterialLayoutSlot& data);
