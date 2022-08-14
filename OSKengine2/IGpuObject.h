#pragma once

#include "OSKmacros.h"

#include <string>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Interfaz común para los objetos almacenados en la CPU.
	/// </summary>
	class OSKAPI_CALL IGpuObject {

	public:

		virtual ~IGpuObject() = default;

		/// <summary>
		/// Establece un nombre que lo identifique.
		/// Para debug.
		/// </summary>
		virtual void SetDebugName(const std::string& name) = 0;

	};

}
