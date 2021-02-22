#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"

#include <string>

namespace OSK {

	//Espacios a los que equivale '\t'.
	constexpr auto SPACES_PER_TAB = 4;


	//Devuelve un string según el valor del bool.
	//true -> "TRUE"
	//false -> "FALSE"
	std::string OSKAPI_CALL ToString(const bool& value);


	//Devuelve un string con los datos del vector 2D.
	template <typename T> std::string OSKAPI_CALL ToString(const Vector2_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "}";
	}


	//Devuelve un string con los datos del vector 3D.
	template <typename T> std::string OSKAPI_CALL ToString(const Vector3_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "; " + std::to_string(vec.Z) + "}";
	}

	//Devuelve un string con los datos del vector 4D.
	template <typename T> std::string OSKAPI_CALL ToString(const Vector4_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "; " + std::to_string(vec.Z) + "; " + std::to_string(vec.W) + "}";
	}


	//Devuelve un string con la posición, tamaño y rotación de un transform.
	//std::string OSKAPI_CALL ToString(const Transform& transform);

}