#pragma once

#include "Vector3.hpp"

namespace OSK {

	/// @brief Representa un plano en el espacio 3D.
	struct Plane {

		/// @brief Punto perteneciente al plano.
		Vector3f point;

		/// @brief Vector normal del plano.
		Vector3f normal;

	};

}
