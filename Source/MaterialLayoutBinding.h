#pragma once

#include <string>
#include "ShaderBindingType.h"
#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un binding de un layout.
	/// 
	/// Un binding corresponde a una �nica unidad de informaci�n
	/// que puede enlazarse al shader (uniform buffer, texture, etc...).
	/// 
	/// Varios bindings se pueden agrupar en un MaterialLayoutSlot.
	/// 
	/// Determina:
	///		- El nombre del binding.
	///		- El tipo de binding (UNIFORM, TEXTURE, etc...).
	///		- �ndice del binding en hlsl.
	///		- �ndice del binding (dentro del set) en glsl.
	/// </summary>
	struct MaterialLayoutBinding {

		/// <summary>
		/// Nombre del binding.
		/// Debe ser �nico en un slot, pero pueden haber varios
		/// bindings con el mismo nombre en distintos slots.
		/// </summary>
		std::string name;

		/// <summary> Tipo de binding. </summary>
		ShaderBindingType type;

		/// <summary>
		/// �ndice del binding en el shader .glsl (Vulkan, OpenGL).
		/// Es un �ndice local respecto al slot (al descriptor set).
		/// </summary>
		UIndex32 glslIndex;

		/// <summary>
		/// �ndice del binding en el shader .hlsl (DirectX 12).
		/// Es un �ndice global, independiente del slot al que pertenezca.
		/// </summary>
		UIndex32 hlslIndex;

		/// <summary>
		/// �ndice del descriptor del shader .hlsl.
		/// Interno.
		/// </summary>
		UIndex32 hlslDescriptorIndex;

		/// <summary>
		/// True si el recurso en el shader corresponde a una array
		/// de recursos tama�o indeterminado.
		/// </summary>
		/// 
		/// @example uniform sampler2D images[];
		bool isUnsizedArray = false;

		/// <summary> N�mero de capas del array. </summary>
		/// 
		/// @note Si no es un array, ser� 1.
		USize32 numArrayLayers = 1;

	};

}

template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayoutBinding>(const OSK::GRAPHICS::MaterialLayoutBinding& binding);
