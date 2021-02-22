#pragma once

#include "ECS.h"

#include "Transform.h"

namespace OSK {

	class TransformComponent : public Component {

	public:

		Transform Transform3D {};

	};

}