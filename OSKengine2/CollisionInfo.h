// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"

namespace OSK::COLLISION {

	/// @brief Informaci�n sobre el resultado de la detecci�n
	/// de colisiones entre dos colliders.
	class OSKAPI_CALL CollisionInfo {

	public:

		/// @return Collision info para el caso en el que
		/// no hay colisi�n.
		static CollisionInfo False();

		/// @return Collision info para el caso en el que
		/// no hay colisi�n, pero los colliders de alto nivel
		/// s� que colisionan.
		static CollisionInfo TopLevelOnly();

		/// @return Collision info para el caso en el que
		/// s� que hay colisi�n.
		static CollisionInfo True();

		bool IsColliding() const;
		bool IsTopLevelColliding() const;

	private:

		CollisionInfo(bool isColliding, bool isTopLevelColliding);

		bool isColliding = false;
		bool isTopLevelColliding = false;

	};

}
