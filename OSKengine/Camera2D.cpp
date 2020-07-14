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
		
		projection = glm::ortho(0.0f, TargetSize.X, 0.0f, TargetSize.Y, -1.0f, 1.0f);
	}

	UniformBuffer2D Camera2D::getUBO() {
		return UniformBuffer2D{ projection };
	}

}