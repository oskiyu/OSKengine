#include "Camera2D.h"
#include <ext\matrix_clip_space.hpp>

namespace OSK {

	Camera2D::Camera2D() {
		projection = glm::mat4(1);
	}

	Camera2D::Camera2D(WindowAPI* window) {
		this->window = window;
		TargetSize = Vector2(window->ScreenSizeX, window->ScreenSizeY);
		Update();
	}

	void Camera2D::Update() {
		if (!UseTargetSize)
			TargetSize = window->GetRectangle().GetRectangleSize();
		
		CameraTransform.UpdateModel();
		projection = glm::ortho(CameraTransform.GlobalPosition.X, TargetSize.X, CameraTransform.GlobalPosition.Y, TargetSize.Y, -1.0f, 1.0f);
	}

	Vector2 Camera2D::PointInWindowToPointInWorld(const Vector2& point) const {
		Vector2 relative = Vector2(0);
		relative.X = point.X / window->ScreenSizeX;
		relative.Y = point.Y / window->ScreenSizeY;

		return relative * TargetSize;
	}

}