#include "Transform3D.h"

#include "EntityComponentSystem.h"
#include "OSKengine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "MatrixOperations.hpp"

using namespace OSK;
using namespace OSK::ECS;

const static ECS::EntityComponentSystem* GetEcs() {
	return Engine::GetEcs();
}

Transform3D::Transform3D(ECS::GameObjectIndex owner) : owner(owner) {

}

void Transform3D::SetPosition(const Vector3f& nPosition) {
	m_localPosition = nPosition;
	UpdateModel();
}

void Transform3D::SetScale(const Vector3f& nScale) {
	m_localScale = nScale;

	if (parent && m_inheritScale)
		m_localScale += GetEcs()->GetComponent<Transform3D>(parent).GetLocalScale();

	UpdateModel();
}

void Transform3D::SetRotation(const Quaternion& nRotation) {
	m_localRotation = nRotation;
	UpdateModel();
}

void Transform3D::AddPosition(const Vector3f& positionDelta) {
	SetPosition(m_localPosition + positionDelta);
}

void Transform3D::AddScale(const Vector3f& scaleDelta) {
	SetScale(m_localScale + scaleDelta);
}

void Transform3D::ApplyRotation(const Quaternion& rotationDelta) {
	glm::quat q = m_localRotation.ToGlm();
	q *= rotationDelta.ToGlm();

	SetRotation(Quaternion::FromGlm(glm::normalize(q)));
}

void Transform3D::RotateLocalSpace(float angle, const Vector3f& axis) {
	m_localRotation.Rotate_LocalSpace(angle, axis);
	UpdateModel();
}

void Transform3D::RotateWorldSpace(float angle, const Vector3f& axis) {
	m_localRotation.Rotate_WorldSpace(angle, axis);
	UpdateModel();
}

void Transform3D::AttachToObject(ECS::GameObjectIndex obj) {
	if (childTransforms.ContainsElement(obj))
			return;

	parent = obj;
	GetEcs()->GetComponent<Transform3D>(obj).childTransforms.Insert(owner);
}

void Transform3D::UnAttach() {
	if (owner && parent)
		GetEcs()->GetComponent<Transform3D>(parent).childTransforms.Remove(owner);

	owner = EMPTY_GAME_OBJECT;
}

void Transform3D::UpdateModel() {
	m_matrix = glm::mat4(1.0f);

	if (parent) {
		const Transform3D& parentTransform = GetEcs()->GetComponent<Transform3D>(parent);

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

	for (UIndex64 i = 0; i < childTransforms.GetSize(); i++) {
		Transform3D& child = GetEcs()->GetComponent<Transform3D>(childTransforms[i]);
		
		if (child.parent == owner) {
			child.UpdateModel();
		}
		else {
			childTransforms[i] = childTransforms.Pop();
		}
	}
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

ECS::GameObjectIndex Transform3D::GetParentObject() const {
	return parent;
}

Vector3f Transform3D::GetForwardVector() const {
	return GetRotation().RotateVector(Vector3f(0, 0, 1));
	return Vector3f(GetRotation().ToGlm() * OSK::Vector3f(0, 0, 1).ToGlm()).GetNormalized();
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
