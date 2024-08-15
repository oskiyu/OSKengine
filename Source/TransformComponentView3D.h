#pragma once

#include "IComponentView.h"
#include "Transform3D.h"

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL TransformComponentView3D : public IComponentView {

	public:

		OSK_LINK_COMPONENT_EDITOR_VIEW(ECS::Transform3D);

		explicit TransformComponentView3D(const Vector2f& size);

		void SetPosition(const Vector3f& velocity);
		void SetRotation(const Vector3f& rotation);
		void SetScale(const Vector3f& scale);

	private:

		OSK::UI::TextView* m_positionView = nullptr;
		OSK::UI::TextView* m_rotationView = nullptr;
		OSK::UI::TextView* m_sizeView = nullptr;

	};

}
