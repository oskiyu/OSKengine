#pragma once

#include "OSKtypes.h"

namespace OSK::Collision {

	struct SAT_CollisionInfo {
		bool IsColliding = false;
		Vector3f MinimunTranslationVector;
	};

}
