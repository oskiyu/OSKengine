#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Tabla que contiene los shaders que pueden ser invocados por un renderizado
	/// de trazado de rayos.
	/// 
	/// Contiene cero, una o m�s entradas de shaders de generaci�n de rayos, de contacto m�s cercano
	/// y de fallo.
	/// </summary>
	/// 
	/// @todo Implementaci�n en DirectX 12.
	class OSKAPI_CALL IRtShaderTable {

	public:

		IRtShaderTable() = default;
		virtual ~IRtShaderTable() = default;

		IRtShaderTable(const IRtShaderTable&) = delete;
		IRtShaderTable& operator=(const IRtShaderTable&) = delete;

		template <typename T> T* As() const requires std::is_base_of_v<IRtShaderTable, T> {
			return (T*)this;
		}

	private:

	};

}
