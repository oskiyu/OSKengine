#include "Transform2D.h"
#include <ext\matrix_transform.hpp>

namespace OSK {

	Transform2D::Transform2D() {
		Position = Vector2(0);
		Scale = Vector2(1);
		Rotation = 0;

		GlobalPosition = Vector2(0);
		GlobalScale = Vector2(0);
		GlobalRotation = 0;

		PositionOffset = Vector2(0);
		ScaleOffset = Vector2(0);
		RotationOffset = 0;

		UpdateModel();
	}

	Transform2D::Transform2D(const Vector2& position, const Vector2& scale, const float& rotation) {
		Position = position;
		Scale = scale;
		Rotation = rotation;

		GlobalPosition = Vector2(0);
		GlobalScale = Vector2(0);
		GlobalRotation = 0;

		PositionOffset = Vector2(0);
		ScaleOffset = Vector2(0);
		RotationOffset = 0;

		UpdateModel();
	}

	Transform2D::~Transform2D() {

	}

	void Transform2D::SetPosition(const Vector2& position) {
		Position = position;
		UpdateModel();
	}

	void Transform2D::SetScale(const Vector2& scale) {
		Scale = scale;
		UpdateModel();
	}

	void Transform2D::SetRotation(const float& rotation) {
		Rotation = rotation;
		UpdateModel();
	}

	void Transform2D::AddPosition(const Vector2& positionDelta) {
		SetPosition(Position + positionDelta);
	}

	void Transform2D::AddScale(const Vector2& scaleDelta) {
		SetScale(Scale + scaleDelta);
	}

	void Transform2D::AddRotation(const float& rotationDelta) {
		SetRotation(Rotation + rotationDelta);
	}

	void Transform2D::UpdateModel() {
		GlobalPosition = Position + PositionOffset;
		GlobalRotation = Rotation + RotationOffset;
		if (UseParentScale)
			GlobalScale = Scale + ScaleOffset;
		else
			GlobalScale = Scale;

		ModelMatrix = glm::mat4(1.0f);

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(GlobalPosition.X, GlobalPosition.Y, 0.0f));

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.5f * GlobalPosition.X, 0.5f * GlobalPosition.Y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(GlobalRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-0.5f * GlobalPosition.X, -0.5f * GlobalPosition.Y, 0.0f));

		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(GlobalScale.X, GlobalScale.Y, 1.0f));

		if (isParent) {
			for (uint32_t i = 0; i < ChildTransforms.size(); i++) {
				if (ChildTransforms[i]->ParentTransform == this) {
					ChildTransforms[i]->RotationOffset = GlobalRotation;
					ChildTransforms[i]->PositionOffset = GlobalPosition;
					ChildTransforms[i]->ScaleOffset = GlobalScale;
					ChildTransforms[i]->UpdateModel();
				}
				else {
					ChildTransforms.erase(ChildTransforms.begin() + i);
				}
			}
		}
	}

	void Transform2D::AttachTo(Transform2D* baseTransform) {
		for (auto i : ChildTransforms)
			if (i == baseTransform)
				return;

		ParentTransform = baseTransform;

		baseTransform->ChildTransforms.push_back(this);
		baseTransform->isParent = true;

		isAttached = true;
	}

	void Transform2D::UnAttach() {
		ParentTransform = nullptr;
		isAttached = false;
	}

	Vector4 Transform2D::GetRectangle() const {
		return Vector4(GlobalPosition.X, GlobalPosition.Y, GlobalScale.X, GlobalScale.Y);
	}

}