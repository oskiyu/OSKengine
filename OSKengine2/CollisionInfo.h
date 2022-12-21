// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"

namespace OSK::COLLISION {

	/// @brief Información sobre el resultado de la detección
	/// de colisiones entre dos colliders.
	class OSKAPI_CALL CollisionInfo {

	public:

		/// @return Collision info para el caso en el que
		/// no hay colisión.
		static CollisionInfo False();

		/// @return Collision info para el caso en el que
		/// no hay colisión, pero los colliders de alto nivel
		/// sí que colisionan.
		static CollisionInfo TopLevelOnly();

		/// @return Collision info para el caso en el que
		/// sí que hay colisión.
		static CollisionInfo True();

		bool IsColliding() const;
		bool IsTopLevelColliding() const;

	private:

		CollisionInfo(bool isColliding, bool isTopLevelColliding);

		bool isColliding = false;
		bool isTopLevelColliding = false;

	};

}
