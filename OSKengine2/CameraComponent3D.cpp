#include "CameraComponent3D.h"

#include "Assert.h"
#include "OSKengine.h"
#include "Window.h"
#include "Transform3D.h"
#include "Quaternion.h"

#include "OSKengine.h"
#include "Logger.h"

#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

using namespace OSK::ECS;

const OSK::Vector3f CameraComponent3D::worldUpVector = { 0.0f, 1.0f, 0.0f };

float CameraComponent3D::GetFov() const {
	return fov;
}

void CameraComponent3D::SetFov(float nfov) {
	if (nfov < fovLimitDown) {
		//OSK_CHECK(false, "Se ha intentado introducir un valor FOV por debajo del límite.");
		fov = fovLimitDown;
	}
	else if (nfov > fovLimitUp) {
		//OSK_CHECK(false, "Se ha intentado introducir un valor FOV por encima del límite.");
		fov = fovLimitUp;
	}
	else {
		fov = nfov;
	}
}

void CameraComponent3D::AddFov(float diff) {
	SetFov(fov + diff);
}

float CameraComponent3D::GetNearPlane() const {
	return nearPlane;
}

float CameraComponent3D::GetFarPlane() const {
	return farPlane;
}

void CameraComponent3D::SetPlanes(float near, float far) {
	OSK_ASSERT(near < far, "Se han intentado introducir valores del plano de cámara inválidos.");

	nearPlane = near;
	farPlane = far;
}

void CameraComponent3D::SetFovLimits(float min, float max) {
	OSK_ASSERT(min < max, "Se han intentado introducir valores del plano de cámara inválidos.");

	fovLimitDown = min;
	fovLimitUp = max;
}

void CameraComponent3D::Rotate(float angleX, float angleY) {
	auto prevAccumAngles = accumulatedAngles;
	angles = Vector2f(angleX, angleY) * 0.25f;
	accumulatedAngles += angles;

	
	const static float middleAngle = 0.0f;
	const static float diffAngle = 180.0f / 2;

	if (accumulatedAngles.Y > middleAngle + diffAngle)
		angles.Y = middleAngle + diffAngle - prevAccumAngles.Y;
	if (accumulatedAngles.Y < middleAngle - diffAngle)
		angles.Y = middleAngle - diffAngle - prevAccumAngles.Y;

	accumulatedAngles = prevAccumAngles + Vector2f(angles.X, angles.Y);
}

void CameraComponent3D::UpdateTransform(Transform3D* transform) {
	transform->RotateWorldSpace(glm::radians(-angles.Y), transform->GetRightVector());
	transform->RotateWorldSpace(glm::radians(-angles.X), worldUpVector);
}

glm::mat4 CameraComponent3D::GetProjectionMatrix(const Transform3D& transform) const {
	return glm::lookAt(transform.GetPosition().ToGLM(), (transform.GetPosition() + transform.GetForwardVector()).ToGLM(), transform.GetTopVector().ToGLM());
}

glm::mat4 CameraComponent3D::GetViewMatrix() const {
	return glm::perspective(glm::radians(fov), Engine::GetWindow()->GetScreenRatio(), nearPlane, farPlane);
}
