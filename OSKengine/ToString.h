#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"

#include <string>

namespace OSK {

	/// <summary>
	/// Espacios a los que equivale '\t' al renderizar texto.
	/// </summary>
	constexpr auto SPACES_PER_TAB = 4;


	/// <summary>
	/// Devuelve un string según el valor del bool.
	/// false -> "FALSE"	
	/// true -> "TRUE"
	/// </summary>
	/// <param name="value">Boolean.</param>
	/// <returns>String.</returns>
	std::string OSKAPI_CALL ToString(const bool& value);

	/// <summary>
	/// Devuelve un string con los datos del vector 2D.
	/// </summary>
	/// <param name="vec">Vector 2D.</param>
	/// <returns>{ X; Y; Z }</returns>
	template <typename T> std::string ToString(const Vector2_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "}";
	}

	/// <summary>
	/// Devuelve un string con los datos del vector 3D.
	/// </summary>
	/// <param name="vec">Vector 3D.</param>
	/// <returns>{ X; Y; Z }</returns>
	template <typename T> std::string ToString(const Vector3_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "; " + std::to_string(vec.Z) + "}";
	}

	/// <summary>
	/// Devuelve un string con los datos del vector 4D.
	/// </summary>
	/// <param name="vec">Vector 4D.</param>
	/// <returns>{ X; Y; Z; W }</returns>
	template <typename T> std::string ToString(const Vector4_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "; " + std::to_string(vec.Z) + "; " + std::to_string(vec.W) + "}";
	}

}