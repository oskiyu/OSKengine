#pragma once

#include "ApiCall.h"
#include <string>

namespace OSK::GRAPHICS {

	enum class ShaderStage;

	/// <summary>
	/// Un push constant permite enviar datos a la GPU directamente
	/// en la lista de comandos.
	/// </summary>
	struct MaterialLayoutPushConstant {

		/// <summary>
		/// Nombre del push constant.
		/// </summary>
		std::string name;

		/// <summary>
		/// Stage del shader desde el que se accede al slot (VERTEX/FRAGMENT).
		/// </summary>
		ShaderStage stage;

		/// <summary>
		/// Tamaño de la estructura que se enviará a la GPU.
		/// </summary>
		USize32 size;

		/// <summary>
		/// Offset desde el primer push constant.
		/// 
		/// @note Cada push constant se colocará después del fin del
		/// anterior push constant.
		/// </summary>
		USize32 offset = 0;

		/// <summary>
		/// Índice del buffer constante donde los shaders
		/// .hlsl recibirán la información.
		/// </summary>
		UIndex32 hlslIndex;

		/// <summary>
		/// Índice interno de DX12, a la hora de bindear.
		/// Interno.
		/// </summary>
		UIndex32 hlslBindingIndex;

	};

}
