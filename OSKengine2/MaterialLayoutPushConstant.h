#pragma once

#include "OSKmacros.h"
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
		/// Tama�o de la estructura que se enviar� a la GPU.
		/// </summary>
		TSize size;

		/// <summary>
		/// Offset desde el primer push constant.
		/// Cada push constant se colocar� despu�s del fin del
		/// anterior push constant.
		/// </summary>
		TSize offset = 0;

		/// <summary>
		/// �ndice del buffer constante donde los shaders
		/// .hlsl recibir�n la informaci�n.
		/// </summary>
		TSize hlslIndex;

		/// <summary>
		/// �ndice interno de DX12, a la hora de bindear.
		/// Interno.
		/// </summary>
		TSize hlslBindingIndex;

	};

}
