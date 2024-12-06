#pragma once

#include <string>
#include "ShaderBindingType.h"
#include "NumericTypes.h"

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

		/// <summary> Tipo de binding. </summary>
		ShaderBindingType type;

		/// <summary>
		/// Índice del binding en el shader .glsl (Vulkan, OpenGL).
		/// Es un índice local respecto al slot (al descriptor set).
		/// </summary>
		UIndex32 glslIndex;

		/// <summary>
		/// Índice del binding en el shader .hlsl (DirectX 12).
		/// Es un índice global, independiente del slot al que pertenezca.
		/// </summary>
		UIndex32 hlslIndex;

		/// <summary>
		/// Índice del descriptor del shader .hlsl.
		/// Interno.
		/// </summary>
		UIndex32 hlslDescriptorIndex;

		/// <summary>
		/// True si el recurso en el shader corresponde a una array
		/// de recursos tamaño indeterminado.
		/// </summary>
		/// 
		/// @example uniform sampler2D images[];
		bool isUnsizedArray = false;

		/// <summary> Número de capas del array. </summary>
		/// 
		/// @note Si no es un array, será 1.
		USize32 numArrayLayers = 1;

	};

}

template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayoutBinding>(const OSK::GRAPHICS::MaterialLayoutBinding& binding);
