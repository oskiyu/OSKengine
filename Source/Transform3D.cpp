#include "Transform3D.h"

#include "EntityComponentSystem.h"
#include "OSKengine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "MatrixOperations.hpp"

#include "Math.h"


using namespace OSK;
using namespace OSK::ECS;

const static ECS::EntityComponentSystem* GetEcs() {
	return Engine::GetEcs();
}


Transform3D Transform3D::FromMatrix(ECS::GameObjectIndex owner, const glm::mat4& matrix) {
	Transform3D output(owner);

	// todo

	return output;
}


Transform3D::Transform3D(ECS::GameObjectIndex owner) : m_ownerId(owner) {

}

void Transform3D::SetPosition(const Vector3f& nPosition) {
	AddPosition(nPosition - m_localPosition);
}

void Transform3D::SetScale(const Vector3f& nScale) {
	AddScale(nScale - m_localScale);

	// if (parent && m_inheritScale)
	//	m_localScale += GetEcs()->GetComponent<Transform3D>(parent).GetLocalScale();
}

void Transform3D::SetRotation(const Quaternion& nRotation) {
	ApplyRotation(nRotation - m_localRotation);
}

void Transform3D::AddPosition(const Vector3f& positionDelta) {
	m_changeInPosition += positionDelta;
	m_isPositionDirty = true;
}

void Transform3D::AddScale(const Vector3f& scaleDelta) {
	m_changeInScale += scaleDelta;
	m_isScaleDirty = true;
}

void Transform3D::ApplyRotation(const Quaternion& rotationDelta) {
	m_changeInRotation += rotationDelta;
	m_isRotationDirty = true;
}

void Transform3D::RotateLocalSpace(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_LocalSpace(angle, axis);
	const Quaternion difference = copy - m_localRotation;

	ApplyRotation(difference);
	// m_isRotationDirty = true;
	// m_localRotation.Rotate_LocalSpace(angle, axis);
}

void Transform3D::RotateWorldSpace(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_WorldSpace(angle, axis);
	const Quaternion difference = copy - m_localRotation;

	ApplyRotation(difference);
	// m_isRotationDirty = true;
	// m_localRotation.Rotate_WorldSpace(angle, axis);
}

void Transform3D::AddChild(ECS::GameObjectIndex obj) {
	if (!m_childIds.ContainsElement(obj)) {
		m_childIds.Insert(obj);
	}
}

void Transform3D::RemoveChild(GameObjectIndex obj) {
	m_childIds.Remove(obj);
}

std::span<const GameObjectIndex> Transform3D::GetChildren() const {
	return m_childIds.GetFullSpan();
}


Vector3f Transform3D::TransformPoint(const Vector3f& point) const {
	return Math::TransformPoint(point, m_matrix);
}

Vector3f Transform3D::GetPosition() const {
	return m_globalPosition;
}

Vector3f Transform3D::GetScale() const {
	return m_globalScale;
}

Vector3f Transform3D::GetLocalPosition() const {
	return m_localPosition;
}

Vector3f Transform3D::GetLocalScale() const {
	return m_localScale;
}

Quaternion Transform3D::GetLocalRotation() const {
	return m_localRotation;
}

Quaternion Transform3D::GetRotation() const {
	return Quaternion::FromGlm(glm::toQuat(m_globalRotation));
}

glm::mat4 Transform3D::GetAsMatrix() const {
	return m_matrix;
}

void Transform3D::OverrideMatrix(const glm::mat4& matrix) {
	m_matrix = matrix;
}

Vector3f Transform3D::GetForwardVector() const {
	return GetRotation().RotateVector(Vector3f(0, 0, 1));
}

Vector3f Transform3D::GetRightVector() const {
	return GetRotation().RotateVector(Vector3f(-1, 0, 0));
}

Vector3f Transform3D::GetTopVector() const {
	return GetRotation().RotateVector(Vector3f(0, 1, 0));
}

void Transform3D::SetShouldInheritPosition(bool value) {
	m_inheritPosition = value;
}

void Transform3D::SetShouldInheritRotation(bool value) {
	m_inheritRotation = value;
}

void Transform3D::SetShouldInheritScale(bool value) {
	m_inheritScale = value;
}

void Transform3D::_ApplyChanges(std::optional<const Transform3D*> parent) {
	const bool isAnyDirty =
		m_isPositionDirty ||
		m_isRotationDirty ||
		m_isScaleDirty;

	if (m_isPositionDirty) {
		m_localPosition += m_changeInPosition;
		m_changeInPosition = Vector3f::Zero;

		m_isPositionDirty = false;
	}

	if (m_isRotationDirty) {
		m_localRotation += m_changeInRotation;
		m_changeInRotation = Quaternion::Empty();

		m_isRotationDirty = false;
	}

	if (m_isScaleDirty) {
		m_localScale += m_changeInScale;
		m_changeInScale = Vector3f::Zero;

		m_isScaleDirty = false;
	}

	if (isAnyDirty) {
		UpdateModel(parent);
	}
}

void Transform3D::UpdateModel(std::optional<const Transform3D*> parent) {
	m_matrix = glm::mat4(1.0f);

	if (parent.has_value()) {
		const Transform3D& parentTransform = *parent.value();

		m_globalRotation = m_inheritRotation
			? parentTransform.m_globalRotation * m_localRotation.ToMat4()
			: m_localRotation.ToMat4();

		if (m_inheritPosition) {
			m_matrix = glm::translate(m_matrix, parentTransform.GetPosition().ToGlm());
		}

		if (m_inheritRotation) {
			m_matrix = m_matrix * parentTransform.m_globalRotation;
		}
	}
	else {
		m_globalRotation = m_localRotation.ToMat4();
	}

	// Posición local.
	m_matrix = glm::translate(m_matrix, m_localPosition.ToGlm());

	// Rotación local.
	m_matrix = m_matrix * m_localRotation.ToMat4();

	// Escala local.
	m_matrix = glm::scale(m_matrix, m_localScale.ToGlm());


	// Obtener posición final.
	m_globalPosition = Vector3f(m_matrix * glm::vec4(0, 0, 0, 1));
}


template <>
nlohmann::json PERSISTENCE::SerializeJson<OSK::ECS::Transform3D>(const OSK::ECS::Transform3D& data) {
	nlohmann::json output{};

	output["m_inheritPosition"] = data.m_inheritPosition;
	output["m_inheritRotation"] = data.m_inheritRotation;
	output["m_inheritScale"] = data.m_inheritScale;

	output["m_matrix"] = SerializeJson<glm::mat4>(data.GetAsMatrix());

	// @todo children

	return output;
}

template <>
OSK::ECS::Transform3D PERSISTENCE::DeserializeJson<OSK::ECS::Transform3D>(const nlohmann::json& json) {
	Transform3D output = Transform3D::FromMatrix(json["owner"], DeserializeJson<glm::mat4>(json["m_matrix"]));

	output.m_inheritPosition = json["m_inheritPosition"];
	output.m_inheritRotation = json["m_inheritRotation"];
	output.m_inheritScale = json["m_inheritScale"];

	return output;
}
