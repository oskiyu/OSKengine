#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Material para el renderizado 3D.
	struct OSKAPI_CALL Material	{

		//OpenGL.
		OSK_INFO_DO_NOT_TOUCH
			int32_t Diffuse = 0;

		//OpenGL.
		OSK_INFO_DO_NOT_TOUCH
			int32_t Specular = 1;

		//Color base del material.
		Color Ambient = Color(1.0f, 1.0f, 1.0f);

		//'Refletancia' del material.
		float_t Shininess = 16.0f;

	};

}
