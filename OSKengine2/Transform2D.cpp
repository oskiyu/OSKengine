#include "Transform2D.h"
#include <glm/ext/matrix_transform.hpp>
#include "OSKengine.h"
#include "EntityComponentSystem.h"

using namespace OSK;
using namespace OSK::ECS;

Transform2D::Transform2D(ECS::GameObjectIndex owner) : owner(owner) {
	localPosition = Vector2f::Zero;
	localScale = Vector2f::One;
	localRotation = 0;

	globalPosition = Vector2f::Zero;
	globalScale = Vector2f::Zero;
	globalRotation = 0;

	positionOffset = Vector2f::Zero;
	scaleOffset = Vector2f::Zero;
	rotationOffset = 0;

	UpdateModel();
}

Transform2D::Transform2D(ECS::GameObjectIndex owner, const Vector2f& position, const Vector2f& scale, float rotation) : owner(owner) {
	this->localPosition = position;
	this->localScale = scale;
	this->localRotation = rotation;

	globalPosition = Vector2f::Zero;
	globalScale = Vector2f::Zero;
	globalRotation = 0.0f;

	positionOffset = Vector2f::Zero;
	scaleOffset = Vector2f::Zero;
	rotationOffset = 0.0f;

	UpdateModel();
}

void Transform2D::SetPosition(const Vector2f& position) {
	localPosition = position;
	UpdateModel();
}

void Transform2D::SetScale(const Vector2f& scale) {
	localScale = scale;
	UpdateModel();
}

void Transform2D::SetRotation(float rotation) {
	localRotation = rotation;
	UpdateModel();
}

void Transform2D::AddPosition(const Vector2f& positionDelta) {
	SetPosition(localPosition + positionDelta);
}

void Transform2D::AddScale(const Vector2f& scaleDelta) {
	SetScale(localScale + scaleDelta);
}

void Transform2D::AddRotation(float rotationDelta) {
	SetRotation(localRotation + rotationDelta);
}

void Transform2D::UpdateModel() {
	globalPosition = localPosition + positionOffset;
	globalRotation = localRotation + rotationOffset;
	globalScale = localScale + scaleOffset;

	modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::translate(modelMatrix, glm::vec3(globalPosition.x, globalPosition.y, 0.0f));

	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f * globalScale.x, 0.5f * globalScale.y, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(globalRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f * globalScale.x, -0.5f * globalScale.y, 0.0f));

	modelMatrix = glm::scale(modelMatrix, glm::vec3(globalScale.x, globalScale.y, 1.0f));

	DynamicArray<GameObjectIndex> childrenToRemove;
	for (UIndex32 i = 0; i < childTransforms.GetSize(); i++) {
		Transform2D& child = Engine::GetEcs()->GetComponent<Transform2D>(childTransforms.At(i));
		if (child.parent == owner) {
			child.rotationOffset = globalRotation;
			child.positionOffset = globalPosition;
			child.scaleOffset = globalScale;
			child.UpdateModel();
		}
		else {
			childrenToRemove.Push(child.owner);
		}
	}

	for (const auto& i : childrenToRemove)
		childTransforms.Remove(i);
}

void Transform2D::AttachToObject(GameObjectIndex baseTransform) {
	for (auto c : childTransforms)
		if (baseTransform == c)
			return;

	parent = baseTransform;
	Engine::GetEcs()->GetComponent<Transform2D>(baseTransform).childTransforms.Insert(owner);
}

void Transform2D::UnAttach() {
	if (owner && parent)
		Engine::GetEcs()->GetComponent<Transform2D>(parent).childTransforms.Remove(owner);

	owner = EMPTY_GAME_OBJECT;
}

Vector4 Transform2D::GetRectangle() const {
	return Vector4(globalPosition.x, globalPosition.y, globalScale.x, globalScale.y);
}

Vector2f Transform2D::GetPosition() const {
	return globalPosition;
}

Vector2f Transform2D::GetScale() const {
	return globalScale;
}

float Transform2D::GetRotation() const {
	return globalRotation;
}

Vector2f Transform2D::GetLocalPosition() const {
	return localPosition;
}

Vector2f Transform2D::GetLocalScale() const {
	return localScale;
}

float Transform2D::GetLocalRotation() const {
	return localRotation;
}

glm::mat4 Transform2D::GetAsMatrix() const {
	return modelMatrix;
}

GameObjectIndex Transform2D::GetParentObject() const {
	return parent;
}

Vector2f Transform2D::GetPositionOffset() const {
	return positionOffset;
}

Vector2f Transform2D::GetScaleOffset() const {
	return scaleOffset;
}

float Transform2D::GetRotationOffset() const {
	return rotationOffset;
}
