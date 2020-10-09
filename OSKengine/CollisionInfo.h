#pragma once

#include "OSKtypes.h"

namespace OSK {

	struct CollisionInfo {

		bool Collision = false;
		float_t Distance = 0.0f;

		Vector3f A_to_B_Direction = { 0.0f };
		Vector3f B_to_A_Direction = { 0.0f };
		Vector3f CollisionPoint = { 0.0f };

	};

}
