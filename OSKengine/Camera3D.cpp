#include "Camera3D.h"

#include <ext\matrix_transform.hpp>
#include <glm.hpp>
#include <ext\matrix_clip_space.hpp>

namespace OSK {

	Camera3D::Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ) {
		CameraTransform = Transform();
		CameraTransform.LocalPosition = Vector3f(posX, posY, posZ);
		CameraTransform.LocalScale = Vector3f(1.0f);

		updateVectors();
	}

	Camera3D::Camera3D(const Vector3f& position, const Vector3f & up) {
		CameraTransform = Transform();
		CameraTransform.LocalPosition = position;
		CameraTransform.LocalScale = Vector3f(1.0f);
		this->Up = up;

		updateVectors();
	}

	Camera3D::~Camera3D() {

	}

	void Camera3D::Girar(double xoffset, double yoffset, bool constraint) {
		float Sensitivity = 0.25f;

		AngleX += xoffset * Sensitivity;
		AngleY -= yoffset * Sensitivity;

		if (constraint) {
			if (AngleY > 89.0f) {
				AngleY = 89.0f;
			}
			else if (AngleY < -89.0f) {
				AngleY = -89.0f;
			}
		}

		CameraTransform.RotateWorldSpace(AngleX, Vector3f(0, 1, 0));
		CameraTransform.RotateWorldSpace(AngleY, Vector3f(1, 0, 0));

		updateVectors();
	}

	void Camera3D::SetFoV(double fov) {
		FieldOfView = fov;
		if (FieldOfView <= FoVLimitDown)
			FieldOfView = FoVLimitDown;
		if (FieldOfView > FoVLimitUp)
			FieldOfView = FoVLimitUp;
	}

	void Camera3D::AddFoV(double fov) {
		SetFoV(FieldOfView - fov);
	}

	void Camera3D::updateVectors() {
		CameraTransform.UpdateModel();

		Vector3f front;
		front.X = cos(glm::radians(AngleY)) * cos(glm::radians(AngleX));
		front.Y = sin(glm::radians(AngleY));
		front.Z = cos(glm::radians(AngleY)) * sin(glm::radians(AngleX));

		Front = front.GetNormalized();
		Right = Front.Cross(WorldUp).GetNormalized();
		Up = Right.Cross(Front).GetNormalized();
	}

	glm::mat4 Camera3D::GetView() const {
		return glm::lookAt(CameraTransform.GlobalPosition.ToGLM(), (CameraTransform.GlobalPosition + Front).ToGLM(), Up.ToGLM());
	}

	glm::mat4 Camera3D::GetProjection() const {
		glm::mat4 proj = glm::perspective(glm::radians(static_cast<float_t>(FieldOfView)), Window->ScreenRatio, 0.1f, 50000.0f);

		return proj;
	}

}