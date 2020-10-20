#include "OldCamera3D.h"

namespace OSK {
	/*
	OldCamera3D::OldCamera3D(const cameraVar_t& posX, const cameraVar_t& posY, const cameraVar_t posZ) {
		CameraTransform = Transform();
		CameraTransform.Position = Vector3(posX, posY, posZ);
		CameraTransform.Scale = Vector3(1.0f);

		updateVectors();
	}


	OldCamera3D::OldCamera3D(const Vector3& position, const Vector3& up) {
		CameraTransform = Transform();
		CameraTransform.Position = position;
		CameraTransform.Scale = Vector3(1.0f);
		this->Up = up;

		updateVectors();
	}


	OldCamera3D::~OldCamera3D() {

	}
	

	void OldCamera3D::Move(const Directions& direction, const deltaTime_t& deltaTime) {
		cameraVar_t velocity = Speed * deltaTime;
		if (direction == OSK::Directions::FORWARD)
			CameraTransform.AddPosition(Front * velocity);
		if (direction == OSK::Directions::BACKWARDS)
			CameraTransform.AddPosition(-Front * velocity);
		if (direction == OSK::Directions::LEFT)
			CameraTransform.AddPosition(-Right * velocity);
		if (direction == OSK::Directions::RIGHT)
			CameraTransform.AddPosition(Right * velocity);

		CameraTransform.UpdateModel();
	}


	void OldCamera3D::Girar(const double& xoffset, const double& yoffset, const bool& constraint) {
		Vector3f rot = glm::eulerAngles(CameraTransform.Rotation.Quat);

		mouseVar_t finalX = rot.X - static_cast<mouseVar_t>(yoffset) * Sensitivity;
		mouseVar_t finalY = rot.Y + static_cast<mouseVar_t>(xoffset) * Sensitivity;

		if (constraint) {
			if (finalX > 89.0f) {
				finalX = 89.0f;
			} else
			if (finalX < -89.0f) {
				finalX = -89.0f;
			}
		}

		CameraTransform.Rotation.FromEulerAngles({ finalX , finalY, 0.0f });
		
		/*CameraTransform.Rotation.X = finalX;
		CameraTransform.Rotation.Y = finalY;

		if (CameraTransform.Rotation.Y > 0.0f)
			CameraTransform.Rotation.Y -= 360.0f;
		
		updateVectors();
	}


	void OldCamera3D::SetFoV(const cameraVar_t& fov) {
		FieldOfView = fov;
		if (FieldOfView <= FoVLimitDown)
			FieldOfView = FoVLimitDown;
		if (FieldOfView > FoVLimitUp)
			FieldOfView = FoVLimitUp;
	}


	void OldCamera3D::AddFoV(const cameraVar_t& fov) {
		SetFoV(FieldOfView - fov);
	}


	void OldCamera3D::updateVectors() {
		CameraTransform.UpdateModel();

		Vector3 front;
		front.X = cos(glm::radians(CameraTransform.GlobalRotation.X)) * cos(glm::radians(CameraTransform.GlobalRotation.Y));
		front.Y = sin(glm::radians(CameraTransform.GlobalRotation.X));
		front.Z = cos(glm::radians(CameraTransform.GlobalRotation.X)) * sin(glm::radians(CameraTransform.GlobalRotation.Y));
		
		Front = front.GetNormalized();
		Right = Front.Cross(WorldUp).GetNormalized();
		Up = Right.Cross(Front).GetNormalized();
	}


	glm::mat4 OldCamera3D::GetView() const {
		return glm::lookAt(CameraTransform.GlobalPosition.ToGLM(), (CameraTransform.GlobalPosition + Front).ToGLM(), Up.ToGLM());
	}


	glm::mat4 OldCamera3D::GetProjection() const {
		return glm::perspective(glm::radians(static_cast<float_t>(FieldOfView)), Window->ScreenRatio, 0.1f, 100.0f);
	}
	*/
}