#pragma once

#include "ISystemView.h"
#include "Vector3.hpp"

#include "PhysicsSystem.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL PhysicsSystemView : public ISystemView {

	public:

		OSK_LINK_SYSTEM_EDITOR_VIEW(ECS::PhysicsSystem);

		explicit PhysicsSystemView(const Vector2f& size);

		void SetGravity(const Vector3f& gravity);

	private:

		OSK::UI::TextView* m_gravityView = nullptr;

	};

}
