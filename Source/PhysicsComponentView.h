#pragma once

#include "IComponentView.h"
#include "PhysicsComponent.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL PhysicsComponentView : public IComponentView {

	public:

		OSK_LINK_COMPONENT_EDITOR_VIEW(ECS::PhysicsComponent);

		explicit PhysicsComponentView(const Vector2f& size);

		void SetMass(float value);
		void SetImmobile();

		void SetVelocity(const Vector3f& velocity);
		void SetAcceleration(const Vector3f& acceleration);

	private:

		OSK::UI::TextView* m_massView = nullptr;
		OSK::UI::TextView* m_velocityView = nullptr;
		OSK::UI::TextView* m_accelerationView = nullptr;

	};

}
