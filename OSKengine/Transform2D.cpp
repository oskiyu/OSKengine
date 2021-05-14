#include "Transform2D.h"
#include <ext\matrix_transform.hpp>

namespace OSK {

	Transform2D::Transform2D() {
		position = Vector2(0);
		scale = Vector2(1);
		rotation = 0;

		globalPosition = Vector2(0);
		globalScale = Vector2(0);
		globalRotation = 0;

		positionOffset = Vector2(0);
		scaleOffset = Vector2(0);
		rotationOffset = 0;

		UpdateModel();
	}

	Transform2D::Transform2D(const Vector2& position, const Vector2& scale, float rotation) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;

		globalPosition = Vector2(0);
		globalScale = Vector2(0);
		globalRotation = 0;

		positionOffset = Vector2(0);
		scaleOffset = Vector2(0);
		rotationOffset = 0;

		UpdateModel();
	}

	void Transform2D::SetPosition(const Vector2& position) {
		this->position = position;

		UpdateModel();
	}

	void Transform2D::SetScale(const Vector2& scale) {
		this->scale = scale;

		UpdateModel();
	}

	void Transform2D::SetRotation(float rotation) {
		this->rotation = rotation;

		UpdateModel();
	}

	void Transform2D::AddPosition(const Vector2& positionDelta) {
		SetPosition(position + positionDelta);
	}

	void Transform2D::AddScale(const Vector2& scaleDelta) {
		SetScale(scale + scaleDelta);
	}

	void Transform2D::AddRotation(float rotationDelta) {
		SetRotation(rotation + rotationDelta);
	}

	void Transform2D::UpdateModel() {
		globalPosition = position + positionOffset;
		globalRotation = rotation + rotationOffset;
		if (useParentScale)
			globalScale = scale + scaleOffset;
		else
			globalScale = scale;

		modelMatrix = glm::mat4(1.0f);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(globalPosition.X, globalPosition.Y, 0.0f));

		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f * globalPosition.X, 0.5f * globalPosition.Y, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(globalRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f * globalPosition.X, -0.5f * globalPosition.Y, 0.0f));

		modelMatrix = glm::scale(modelMatrix, glm::vec3(globalScale.X, globalScale.Y, 1.0f));

		if (isParent) {
			for (uint32_t i = 0; i < childTransforms.size(); i++) {
				if (childTransforms[i]->parentTransform == this) {
					childTransforms[i]->rotationOffset = globalRotation;
					childTransforms[i]->positionOffset = globalPosition;
					childTransforms[i]->scaleOffset = globalScale;
					childTransforms[i]->UpdateModel();
				}
				else {
					childTransforms.erase(childTransforms.begin() + i);
				}
			}
		}
	}

	void Transform2D::AttachTo(Transform2D* baseTransform) {
		for (auto i : childTransforms)
			if (i == baseTransform)
				return;

		parentTransform = baseTransform;

		baseTransform->childTransforms.push_back(this);
		baseTransform->isParent = true;

		isAttached = true;
	}

	void Transform2D::UnAttach() {
		parentTransform = nullptr;
		isAttached = false;
	}

	Vector4 Transform2D::GetRectangle() const {
		return Vector4(globalPosition.X, globalPosition.Y, globalScale.X, globalScale.Y);
	}

	Vector2 Transform2D::GetPosition() const {
		return globalPosition;
	}

	Vector2 Transform2D::GetScale() const {
		return globalScale;
	}

	float Transform2D::GetRotation() const {
		return globalRotation;
	}

	Vector2 Transform2D::GetLocalPosition() const {
		return position;
	}

	Vector2 Transform2D::GetLocalScale() const {
		return scale;
	}

	float Transform2D::GetLocalRotation() const {
		return rotation;
	}

	glm::mat4 Transform2D::AsMatrix() const {
		return modelMatrix;
	}

	Transform2D* Transform2D::GetParent() {
		return parentTransform;
	}

	std::vector<Transform2D*> Transform2D::GetChildTransforms() const {
		return childTransforms;
	}

	Vector2 Transform2D::GetPositionOffset() const {
		return positionOffset;
	}

	Vector2 Transform2D::GetScaleOffset() const {
		return scaleOffset;
	}

	float Transform2D::GetRotationOffset() const {
		return rotationOffset;
	}

	bool Transform2D::IsUsingParentScale() const {
		return useParentScale;
	}

	void Transform2D::SetShoulduseParentScale(bool useParentScale) {
		this->useParentScale = useParentScale;
	}

}