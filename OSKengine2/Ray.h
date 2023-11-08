// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"

namespace OSK::COLLISION {
	
	/// @brief Representa un rayo lanzado en el mundo que puede
	/// colisionar con colliders.
	struct Ray {

		/// @brief Posición del mundo desde el que se origina el rayo.
		Vector3f origin;

		/// @brief Dirección del rayo.
		Vector3f direction;

		/// @brief Longitud máxima que puede llegar a tener el rayo.
		/// No se tendrán en cuenta las colisiones con colliders que
		/// estén más lejos que la longitud del rayo.
		float maxLength = 1000.0f;

	};

}
