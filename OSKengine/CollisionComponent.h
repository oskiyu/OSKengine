#pragma once

#include "ECS.h"

#include "Collider.h"

namespace OSK {

	class CollisionComponent : public Component {

	public:

		Collider Collider3D;

	};

}