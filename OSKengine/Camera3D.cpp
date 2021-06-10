#include "Camera3D.h"

#include <ext\matrix_transform.hpp>
#include <glm.hpp>
#include <ext\matrix_clip_space.hpp>

namespace OSK {

	Camera3D::Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ) {
		transform = Transform();
		transform.SetPosition(Vector3f((float)posX, (float)posY, (float)posZ));
		transform.SetScale(Vector3f(1.0f));

		updateVectors();
	}

	Camera3D::Camera3D(const Vector3f& position, const Vector3f& up) {
		transform = Transform();
		transform.SetPosition(position);
		transform.SetScale(Vector3f(1.0f));
		this->up = up;

		updateVectors();
	}

	Camera3D::~Camera3D() {

	}

	void Camera3D::Girar(double xoffset, double yoffset, bool constraint) {
		constexpr float sensitivity = 0.25f;

		angleX += (float)xoffset * sensitivity;
		angleY -= (float)yoffset * sensitivity;

		if (constraint) {
			if (angleY > 89.0f) {
				angleY = 89.0f;
			}
			else if (angleY < -89.0f) {
				angleY = -89.0f;
			}
		}

		transform.RotateWorldSpace(angleX, Vector3f(0, 1, 0));
		transform.RotateWorldSpace(angleY, Vector3f(1, 0, 0));

		updateVectors();
	}

	void Camera3D::SetFov(double fov) {
		fieldOfView = (float)fov;
		if (fieldOfView <= fovLimitDown)
			fieldOfView = fovLimitDown;
		if (fieldOfView > fovLimitUp)
			fieldOfView = fovLimitUp;
	}

	void Camera3D::AddFov(double fov) {
		SetFov(fieldOfView - fov);
	}

	void Camera3D::updateVectors() {
		transform.UpdateModel();

		Vector3f front_;
		front_.X = cos(glm::radians(angleY)) * cos(glm::radians(angleX));
		front_.Y = sin(glm::radians(angleY));
		front_.Z = cos(glm::radians(angleY)) * sin(glm::radians(angleX));

		front = front_.GetNormalized();
		right = front.Cross(worldUp).GetNormalized();
		up = right.Cross(front).GetNormalized();
	}

	Transform* Camera3D::GetTransform() {
		return &transform;
	}

	glm::mat4 Camera3D::GetView() const {
		return glm::lookAt(transform.GetPosition().ToGLM(), (transform.GetPosition() + front).ToGLM(), up.ToGLM());
	}

	glm::mat4 Camera3D::GetProjection() const {
		glm::mat4 proj = glm::perspective(glm::radians(static_cast<float_t>(fieldOfView)), window->GetScreenAspectRatio(), 0.1f, 50000.0f);

		return proj;
	}

	Vector3f Camera3D::GetFrontVector() const {
		return front;
	}

	Vector3f Camera3D::GetUpVector() const {
		return up;
	}

	Vector3f Camera3D::GetRightVector() const {
		return right;
	}

}