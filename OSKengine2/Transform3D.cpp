#include "Transform3D.h"

#include "EntityComponentSystem.h"
#include "OSKengine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

using namespace OSK;
using namespace OSK::ECS;

const static ECS::EntityComponentSystem* GetEcs() {
	return Engine::GetEcs();
}

Transform3D::Transform3D(ECS::GameObjectIndex owner) : owner(owner) {
	matrix = glm::mat4(1);

	localPosition = Vector3f(0);
	localScale = Vector3f(1);

	globalPosition = Vector3f(0);
	globalScale = Vector3f(1);

	parent = 0;
}

void Transform3D::SetPosition(const Vector3f& nPosition) {
	localPosition = nPosition;
	UpdateModel();
}

void Transform3D::SetScale(const Vector3f& nScale) {
	localScale = nScale;

	if (parent)
		localScale += GetEcs()->GetComponent<Transform3D>(parent).GetLocalScale();

	UpdateModel();
}

void Transform3D::SetRotation(const Quaternion& nRotation) {
	localRotation = nRotation;
	UpdateModel();
}

void Transform3D::AddPosition(const Vector3f& positionDelta) {
	SetPosition(localPosition + positionDelta);
}

void Transform3D::AddScale(const Vector3f& scaleDelta) {
	SetScale(localScale + scaleDelta);
}

void Transform3D::ApplyRotation(const Quaternion& rotationDelta) {
	glm::quat q = localRotation.ToGlm();
	q *= rotationDelta.ToGlm();

	SetRotation(Quaternion::FromGlm(q));
}

void Transform3D::RotateLocalSpace(float angle, const Vector3f& axis) {
	localRotation.Rotate_LocalSpace(angle, axis);
	UpdateModel();
}

void Transform3D::RotateWorldSpace(float angle, const Vector3f& axis) {
	localRotation.Rotate_WorldSpace(angle, axis);
	UpdateModel();
}

void Transform3D::AttachToObject(ECS::GameObjectIndex obj) {
	for (auto c : childTransforms)
		if (obj == c)
			return;

	parent = obj;
	GetEcs()->GetComponent<Transform3D>(obj).childTransforms.Insert(owner);
}

void Transform3D::UnAttach() {
	if (owner && parent)
		GetEcs()->GetComponent<Transform3D>(parent).childTransforms.Remove(owner);

	owner = 0;
}

void Transform3D::UpdateModel() {
	matrix = glm::mat4(1.0f);

	if (parent) {
		const Transform3D& parentTr = GetEcs()->GetComponent<Transform3D>(parent);

		globalRotation = parentTr.globalRotation * localRotation.ToMat4();

		matrix = glm::translate(matrix, parentTr.GetPosition().ToGLM());
		matrix *= parentTr.globalRotation;
	}
	else {
		globalRotation = localRotation.ToMat4();
	}

	//Posición local.
	matrix = glm::translate(matrix, localPosition.ToGLM());

	//Escala local.
	matrix = glm::scale(matrix, localScale.ToGLM());

	//Rotación local.
	matrix = matrix * localRotation.ToMat4();

	//Obtener posición final.
	globalPosition = Vector3f(matrix * glm::vec4(0, 0, 0, 1));

	DynamicArray<GameObjectIndex> childrenToRemove;
	for (TSize i = 0; i < childTransforms.GetSize(); i++) {
		Transform3D& child = GetEcs()->GetComponent<Transform3D>(childTransforms[i]);
		if (child.parent == owner)
			child.UpdateModel();
		else
			childrenToRemove.Push(child.owner);
	}

	for (const auto& i : childrenToRemove)
		childTransforms.Remove(i);
}

Vector3f Transform3D::TransformPoint(const Vector3f& point) const {
	const glm::mat4 mat = glm::translate(matrix, point.ToGLM());
	return Vector3f(mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

Vector3f Transform3D::GetPosition() const {
	return globalPosition;
}

Vector3f Transform3D::GetScale() const {
	return globalScale;
}

Vector3f Transform3D::GetLocalPosition() const {
	return localPosition;
}

Vector3f Transform3D::GetLocalScale() const {
	return localScale;
}

Quaternion Transform3D::GetLocalRotation() const {
	return localRotation;
}

Quaternion Transform3D::GetRotation() const {
	return Quaternion::FromGlm(glm::toQuat(globalRotation));
}

glm::mat4 Transform3D::GetAsMatrix() const {
	return matrix;
}

void Transform3D::OverrideMatrix(const glm::mat4& matrix) {
	this->matrix = matrix;
}

ECS::GameObjectIndex Transform3D::GetParentObject() const {
	return parent;
}

Vector3f Transform3D::GetForwardVector() const {
	return Vector3f(GetRotation().ToGlm() * OSK::Vector3f(0, 0, 1).ToGLM()).GetNormalized();
}

Vector3f Transform3D::GetRightVector() const {
	return Vector3f(GetRotation().ToGlm() * OSK::Vector3f(-1, 0, 0).ToGLM()).GetNormalized();
}

Vector3f Transform3D::GetTopVector() const {
	return Vector3f(GetRotation().ToGlm() * OSK::Vector3f(0, 1, 0).ToGLM()).GetNormalized();
}
