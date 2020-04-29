#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Font.h"
#include "Transform.h"

#include <string>

namespace OSK {

	//Espacios a los que equivale '\t'.
	constexpr auto SPACES_PER_TAB = 4;


	//Devuelve un string según el valor del bool.
	//true -> "TRUE"
	//false -> "FALSE"
	OSK_INFO_GLOBAL
		std::string ToString(const bool& value);

	//Devuelve un string con los datos del vector 2D.
	OSK_INFO_GLOBAL
		std::string ToString(const Vector2& vector2);


	//Devuelve un string con los datos del vector 2D.
	OSK_INFO_GLOBAL
		//std::string ToString(const OSK::Vector2i& vector2);


	//Devuelve un string con los datos del vector 3D.
	OSK_INFO_GLOBAL
		std::string ToString(const Vector3& vector3);


	//Devuelve un string con los datos del vector 2D.
	OSK_INFO_GLOBAL
		//std::string ToString(const OSK::Vector3i& vector3);


	//Devuelve un string con los datos del vector 4D.
	OSK_INFO_GLOBAL
		std::string ToString(const Vector4& vector4);


	//Devuelve un string con los datos del vector 2D.
	OSK_INFO_GLOBAL
		//std::string ToString(const OSK::Vector4i& vector4);


	//Devuelve un string con la posición, tamaño y rotación de un transform.
	OSK_INFO_GLOBAL
	OSK_INFO_NOT_DEVELOPED
		std::string ToString(const Transform& transform);


	//Calcula el tamaño que ocupará un texto.
	OSK_INFO_GLOBAL
		Vector2 GetTextSize(const std::string& texto, Font& fuente, const Vector2& size = Vector2(1.0f));
	

	//Calcula el tamaño que ocupará un texto.
	OSK_INFO_GLOBAL
		Vector2 GetTextSize(const std::string& texto, Font& fuente, const float_t& size = 1.0f);

}