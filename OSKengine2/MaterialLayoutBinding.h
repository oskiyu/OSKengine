#pragma once

#include <string>
#include "ShaderBindingType.h"
#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un binding de un layout.
	/// 
	/// Un binding corresponde a una única unidad de información
	/// que puede enlazarse al shader (uniform buffer, texture, etc...).
	/// 
	/// Varios bindings se pueden agrupar en un MaterialLayoutSlot.
	/// 
	/// Determina:
	///		- El nombre del binding.
	///		- El tipo de binding (UNIFORM, TEXTURE, etc...).
	///		- Índice del binding en hlsl.
	///		- Índice del binding (dentro del set) en glsl.
	/// </summary>
	struct MaterialLayoutBinding {

		/// <summary>
		/// Nombre del binding.
		/// Debe ser único en un slot, pero pueden haber varios
		/// bindings con el mismo nombre en distintos slots.
		/// </summary>
		std::string name;

		/// <summary>
		/// Tipo de binding.
		/// </summary>
		ShaderBindingType type;

		/// <summary>
		/// Índice del binding en el shader .glsl (Vulkan, OpenGL).
		/// Es un índice local respecto al slot (al descriptor set).
		/// </summary>
		TSize glslIndex;

		/// <summary>
		/// Índice del binding en el shader .hlsl (DirectX 12).
		/// Es un índice global, independiente del slot al que pertenezca.
		/// </summary>
		TSize hlslIndex;

		/// <summary>
		/// Índice del descriptor del shader .hlsl.
		/// Interno.
		/// </summary>
		TSize hlslDescriptorIndex;

	};

}
