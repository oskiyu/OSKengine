#include "CameraComponent3D.h"

#include "Assert.h"
#include "OSKengine.h"
#include "Window.h"
#include "Transform3D.h"
#include "Quaternion.h"

#include "OSKengine.h"
#include "Logger.h"
#include "Math.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace OSK::ECS;

const OSK::Vector3f CameraComponent3D::worldUpVector = { 0.0f, 1.0f, 0.0f };

float CameraComponent3D::GetFov() const {
	return fov;
}

void CameraComponent3D::SetFov(float nfov) {
	fov = MATH::Clamp(nfov, fovLimitDown, fovLimitUp);
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
	nearPlane = near;
	farPlane = far;
}

void CameraComponent3D::SetFovLimits(float min, float max) {
	fovLimitDown = min;
	fovLimitUp = max;
}

void CameraComponent3D::Rotate(float angleX, float angleY) {
	const Vector2f prevAccumAngles = accumulatedAngles;
	angles = Vector2f(angleX, angleY) * 0.25f;
	accumulatedAngles += angles;

	
	const static float middleAngle = 0.0f;
	const static float diffAngle = 180.0f / 2;

	if (accumulatedAngles.y > middleAngle + diffAngle)
		angles.y = middleAngle + diffAngle - prevAccumAngles.y;
	if (accumulatedAngles.y < middleAngle - diffAngle)
		angles.y = middleAngle - diffAngle - prevAccumAngles.y;

	accumulatedAngles = prevAccumAngles + Vector2f(angles.x, angles.y);
}

void CameraComponent3D::UpdateTransform(Transform3D* transform) {
	transform->RotateLocalSpace(glm::radians(-angles.y), { 1, 0, 0 });
	transform->RotateWorldSpace(glm::radians(-angles.x), worldUpVector);

	angles = Vector2f::Zero;
}

glm::mat4 CameraComponent3D::GetProjectionMatrix() const {
	const float f = 1.0f / glm::tan(glm::radians(fov) * 0.5f);

	return glm::mat4(
		f / Engine::GetDisplay()->GetScreenRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearPlane, 0.0f);
}

glm::mat4 CameraComponent3D::GetProjectionMatrix_UnreversedZ() const {
	return glm::perspectiveRH_ZO<float>(
		glm::radians(fov),
		Engine::GetDisplay()->GetScreenRatio(),
		nearPlane,
		farPlane);
}

glm::mat4 CameraComponent3D::GetViewMatrix(const Transform3D& transform) const {
	return glm::lookAt<float>(
		transform.GetPosition().ToGLM(), 
		(transform.GetPosition() + transform.GetForwardVector()).ToGLM(), 
		transform.GetTopVector().ToGLM());
}
