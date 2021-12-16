#include "Camera2D.h"
#include <ext\matrix_clip_space.hpp>

#include "MaterialSlot.h"

namespace OSK {

	Camera2D::Camera2D(WindowAPI* window) {
		this->window = window;
		projection = glm::mat4(1);
		targetSize = this->window->GetSize().ToVector2f();

		Update();
	}

	Camera2D::~Camera2D() {
		cameraMaterial.Delete();
	}

	Transform2D& Camera2D::GetTransform() {
		return transform;
	}

	void Camera2D::Update() {
		if (!useTargetSize)
			targetSize = window->GetRectangle().GetRectangleSize();
		
		transform.UpdateModel();
		projection = glm::ortho(transform.GetPosition().X, targetSize.X, transform.GetPosition().Y, targetSize.Y, -1.0f, 1.0f);
	}

	Vector2 Camera2D::PointInWindowToPointInWorld(const Vector2& point) const {
		Vector2 relative = Vector2(0);
		relative.X = point.X / window->GetSize().X;
		relative.Y = point.Y / window->GetSize().Y;

		return relative * targetSize;
	}

	glm::mat4 Camera2D::GetProjection() const {
		return projection;
	}

}