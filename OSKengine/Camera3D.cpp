#include "Camera3D.h"

#include <ext\matrix_transform.hpp>
#include <glm.hpp>
#include <ext\matrix_clip_space.hpp>

namespace OSK {

	Camera3D::Camera3D(const cameraVar_t& posX, const cameraVar_t& posY, const cameraVar_t posZ) {
		CameraTransform = Transform_t<double>();
		CameraTransform.Position = Vector3_t<double>(posX, posY, posZ);
		CameraTransform.Scale = Vector3_t<double>(1.0f);

		updateVectors();
	}


	Camera3D::Camera3D(const Vector3_t<double>& position, const Vector3_t<double>& up) {
		CameraTransform = Transform_t<double>();
		CameraTransform.Position = position;
		CameraTransform.Scale = Vector3_t<double>(1.0f);
		this->Up = up;

		updateVectors();
	}

	Camera3D::~Camera3D() {

	}

	void Camera3D::Girar(const double& xoffset, const double& yoffset, const bool& constraint) {
		float Sensitivity = 0.25f;
		mouseVar_t finalX = CameraTransform.Rotation.X - static_cast<mouseVar_t>(yoffset) * Sensitivity;
		mouseVar_t finalY = CameraTransform.Rotation.Y + static_cast<mouseVar_t>(xoffset) * Sensitivity;

		if (constraint) {
			if (finalX > 89.0f) {
				finalX = 89.0f;
			}
			else
				if (finalX < -89.0f) {
					finalX = -89.0f;
				}
		}

		CameraTransform.Rotation.X = finalX;
		CameraTransform.Rotation.Y = finalY;

		if (CameraTransform.Rotation.Y > 0.0f)
			CameraTransform.Rotation.Y -= 360.0f;

		updateVectors();
	}

	void Camera3D::SetFoV(const double& fov) {
		FieldOfView = fov;
		if (FieldOfView <= FoVLimitDown)
			FieldOfView = FoVLimitDown;
		if (FieldOfView > FoVLimitUp)
			FieldOfView = FoVLimitUp;
	}


	void Camera3D::AddFoV(const double& fov) {
		SetFoV(FieldOfView - fov);
	}

	void Camera3D::updateVectors() {
		CameraTransform.UpdateModel();

		Vector3_t<double> front;
		front.X = cos(glm::radians(CameraTransform.GlobalRotation.X)) * cos(glm::radians(CameraTransform.GlobalRotation.Y));
		front.Y = sin(glm::radians(CameraTransform.GlobalRotation.X));
		front.Z = cos(glm::radians(CameraTransform.GlobalRotation.X)) * sin(glm::radians(CameraTransform.GlobalRotation.Y));

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