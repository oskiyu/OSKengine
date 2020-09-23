#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "OldFont.h"
#include "Transform.h"

#include <string>

namespace OSK {

	//Espacios a los que equivale '\t'.
	constexpr auto SPACES_PER_TAB = 4;


	//Devuelve un string según el valor del bool.
	//true -> "TRUE"
	//false -> "FALSE"
	OSK_INFO_GLOBAL
		std::string OSKAPI_CALL ToString(const bool& value);


	//Devuelve un string con los datos del vector 2D.
	OSK_INFO_GLOBAL
		template <typename T> std::string ToString(const Vector2_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "}";
	}


	//Devuelve un string con los datos del vector 3D.
	OSK_INFO_GLOBAL
	template <typename T> std::string ToString(const Vector3_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "; " + std::to_string(vec.Z) + "}";
	}


	//Devuelve un string con los datos del vector 2D.
	OSK_INFO_GLOBAL
		//std::string ToString(const OSK::Vector3i& vector3);


	//Devuelve un string con los datos del vector 4D.
	OSK_INFO_GLOBAL
	template <typename T> std::string ToString(const Vector4_t<T>& vec) {
		return "{ " + std::to_string(vec.X) + "; " + std::to_string(vec.Y) + "; " + std::to_string(vec.Z) + "; " + std::to_string(vec.W) + "}";
	}


	//Devuelve un string con la posición, tamaño y rotación de un transform.
	OSK_INFO_GLOBAL
	OSK_INFO_NOT_DEVELOPED
		std::string OSKAPI_CALL ToString(const Transform& transform);


	//Calcula el tamaño que ocupará un texto.
	OSK_INFO_GLOBAL
		Vector2 OSKAPI_CALL GetTextSize(const std::string& texto, OldFont& fuente, const Vector2& size = Vector2(1.0f));
	

	//Calcula el tamaño que ocupará un texto.
	OSK_INFO_GLOBAL
		Vector2 OSKAPI_CALL GetTextSize(const std::string& texto, OldFont& fuente, const float_t& size = 1.0f);

}