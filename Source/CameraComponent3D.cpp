#include "CameraComponent3D.h"

#include "Assert.h"
#include "OSKengine.h"
#include "Window.h"
#include "TransformComponent3D.h"
#include "Quaternion.h"

#include "OSKengine.h"
#include "Logger.h"
#include "Math.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::PERSISTENCE;

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

AnyFrustum CameraComponent3D::GetFrustum(const Transform3D& transform) const {
	AnyFrustum output{};

	Vector2f nearSize_half = Vector2f::Zero;

	nearSize_half.y = nearPlane * glm::tan(glm::radians(fov) * 0.5f);
	nearSize_half.x = nearSize_half.y * Engine::GetDisplay()->GetScreenRatio();

	const Vector3f up = transform.GetTopVector().GetNormalized();
	const Vector3f right = transform.GetRightVector().GetNormalized();
	const Vector3f forward = transform.GetForwardVector().GetNormalized();

	const Vector3f nearLeftPosition = forward * nearPlane - right * nearSize_half.x;
	const Vector3f nearRightPosition = forward * nearPlane + right * nearSize_half.x;

	const Vector3f nearTopPosition = forward * nearPlane + up * nearSize_half.y;
	const Vector3f nearBottomPosition = forward * nearPlane - up * nearSize_half.y;

	// Near
	output.Insert({
		.point = transform.GetPosition() + forward * nearPlane,
		.normal = forward 
	});

	// Left
	output.Insert({
		.point = transform.GetPosition(),
		.normal = -up.Cross(nearLeftPosition).GetNormalized()
		});

	// Right
	output.Insert({
		.point = transform.GetPosition(),
		.normal = up.Cross(nearRightPosition).GetNormalized()
		});

	// Top
	output.Insert({
		.point = transform.GetPosition(),
		.normal = -right.Cross(nearTopPosition).GetNormalized()
		});

	// Bottom
	output.Insert({
		.point = transform.GetPosition(),
		.normal = right.Cross(nearBottomPosition).GetNormalized()
		});

	return output;
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
		transform.GetPosition().ToGlm(),
		(transform.GetPosition() + transform.GetForwardVector()).ToGlm(),
		transform.GetTopVector().ToGlm());
}



template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::CameraComponent3D>(const OSK::ECS::CameraComponent3D& data) {
	nlohmann::json output{};

	output["fov"] = data.fov;
	output["fovLimitDown"] = data.fovLimitDown;
	output["fovLimitUp"] = data.fovLimitUp;

	output["nearPlane"] = data.nearPlane;
	output["farPlane"] = data.farPlane;

	output["angles"] = SerializeVector2(data.angles);
	output["accumulatedAngles"] = SerializeVector2(data.accumulatedAngles);

	return output;
}

template <>
OSK::ECS::CameraComponent3D PERSISTENCE::DeserializeComponent<OSK::ECS::CameraComponent3D>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	OSK::ECS::CameraComponent3D output{};

	output.fov = json["fov"];
	output.fovLimitDown = json["fovLimitDown"];
	output.fovLimitUp = json["fovLimitUp"];

	output.nearPlane = json["nearPlane"];
	output.farPlane = json["farPlane"];

	output.angles = DeserializeVector2<Vector2f>(json["angles"]);
	output.accumulatedAngles = DeserializeVector2<Vector2f>(json["accumulatedAngles"]);

	return output;
}


template<>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::CameraComponent3D>(const OSK::ECS::CameraComponent3D& data) {
	BinaryBlock output{};

	output.Write(data.fov);
	output.Write(data.fovLimitDown);
	output.Write(data.fovLimitUp);

	output.Write(data.nearPlane);
	output.Write(data.farPlane);

	output.AppendBlock(SerializeBinaryVector2(data.angles));
	output.AppendBlock(SerializeBinaryVector2(data.accumulatedAngles));

	return output;
}

template <>
OSK::ECS::CameraComponent3D PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::CameraComponent3D>(BinaryBlockReader* reader, const SavedGameObjectTranslator&) {
	OSK::ECS::CameraComponent3D output{};

	output.fov = reader->Read<float>();
	output.fovLimitDown = reader->Read<float>();
	output.fovLimitUp = reader->Read<float>();

	output.nearPlane = reader->Read<float>();
	output.farPlane = reader->Read<float>();

	output.angles = DeserializeBinaryVector2<Vector2f, float>(reader);
	output.accumulatedAngles = DeserializeBinaryVector2<Vector2f, float>(reader);

	return output;
}
