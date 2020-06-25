#include "Transform.h"

#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>

namespace OSK {

	Transform::Transform() {
		ModelMatrix = glm::mat4(1);

		Position = Vector3(0);
		Scale = Vector3(0);
		Rotation = Vector3(0);

		GlobalPosition = Vector3(0);
		GlobalScale = Vector3(0);
		GlobalRotation = Vector3(0);

		PositionOffset = Vector3(0);
		ScaleOffset = Vector3(0);
		RotationOffset = Vector3(0);

		isSkeletal = false;
		isAttached = false;
		ParentTransform = nullptr;
	}


	Transform::Transform(const Vector3& position, const Vector3& scale, const Vector3& rotation) {
		ModelMatrix = glm::mat4(1);
		Position = position;
		Scale = scale;
		Rotation = rotation;

		PositionOffset = Vector3(0);
		RotationOffset = Vector3(0);

		isAttached = false;
		ParentTransform = nullptr;

		isSkeletal = false;

		UpdateModel();
	}


	Transform::Transform(const glm::mat4& model) {
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		glm::vec3 skew;
		glm::vec4 prespective;
	
		glm::decompose(model, scale, rotation, position, skew, prespective);

		rotation = glm::conjugate(rotation);

		ModelMatrix = glm::mat4(1);

		Scale = Vector3(1);
		
		GlobalPosition = Vector3(0);
		GlobalScale = Vector3(1);
		GlobalRotation = Vector3(0);

		PositionOffset = Vector3(0);
		ScaleOffset = Vector3(0);
		RotationOffset = Vector3(0);

		Position = Vector3(position);
		Rotation = Vector3(glm::eulerAngles(rotation) * (3.14159f / 2.0f));
				
		isAttached = false;
		ParentTransform = nullptr;

		isSkeletal = true;

		//UpdateModel();

		ModelMatrix = model;
	}


	Transform::~Transform() {
		
	}


	void Transform::SetPosition(const Vector3& position) {
		Position = position;

		UpdateModel();
	}


	void Transform::SetScale(const Vector3& scale) {
		Scale = scale;

		UpdateModel();
	}


	void Transform::SetRotation(const Vector3& rotation) {
		Rotation = rotation;

		UpdateModel();
	}


	void Transform::AddPosition(const Vector3& positionDelta) {
		SetPosition(Position + positionDelta);
	}


	void Transform::AddScale(const Vector3& scaleDelta) {
		SetScale(Scale + scaleDelta);
	}


	void Transform::AddRotation(const Vector3& rotationDelta) {
		SetRotation(Rotation + rotationDelta);
	}


	void Transform::UpdateModel() {
		struct transform {
			Transform* t = nullptr;
			uint32_t nextChild = 0;
		} current{this, 0};

		std::vector<transform> stack;

		uint32_t currentIt = 0;
		const uint32_t maxIt = 10;
		
		while (current.t != nullptr) {
			current.t->GlobalRotation = current.t->Rotation + current.t->RotationOffset;
			current.t->GlobalPosition = current.t->Position + current.t->PositionOffset;
			current.t->GlobalScale = current.t->Scale;
			if (current.t->UseParentScale)
				current.t->GlobalScale = current.t->GlobalScale + current.t->ScaleOffset;

			current.t->ModelMatrix = glm::mat4(1);

			current.t->ModelMatrix = glm::translate(current.t->ModelMatrix, current.t->PositionOffset.ToGLM());

			current.t->ModelMatrix = glm::rotate(current.t->ModelMatrix, glm::radians(current.t->RotationOffset.X), glm::vec3(1.0f, 0.0f, 0.0f));
			current.t->ModelMatrix = glm::rotate(current.t->ModelMatrix, glm::radians(current.t->RotationOffset.Y), glm::vec3(0.0f, 1.0f, 0.0f));
			current.t->ModelMatrix = glm::rotate(current.t->ModelMatrix, glm::radians(current.t->RotationOffset.Z), glm::vec3(0.0f, 0.0f, 1.0f));

			current.t->GlobalPosition = Vector3(current.t->ModelMatrix * Vector4(current.t->Position.X, current.t->Position.Y, current.t->Position.Z, 1.0f).ToGLM());

			current.t->ModelMatrix = glm::translate(current.t->ModelMatrix, current.t->Position.ToGLM());
			current.t->ModelMatrix = glm::scale(current.t->ModelMatrix, current.t->GlobalScale.ToGLM());

			current.t->ModelMatrix = glm::rotate(current.t->ModelMatrix, glm::radians(current.t->Rotation.X), glm::vec3(1.0f, 0.0f, 0.0f));
			current.t->ModelMatrix = glm::rotate(current.t->ModelMatrix, glm::radians(current.t->Rotation.Y), glm::vec3(0.0f, 1.0f, 0.0f));
			current.t->ModelMatrix = glm::rotate(current.t->ModelMatrix, glm::radians(current.t->Rotation.Z), glm::vec3(0.0f, 0.0f, 1.0f));

			currentIt++;
			if (currentIt > maxIt)
				break;

			if (!isSkeletal) {
				if (current.t->isParent) {
					if (current.nextChild < current.t->ChildTransforms.size()) {
						transform next{ current.t->ChildTransforms[current.nextChild], 0 };

						current.nextChild++;
						stack.push_back(current);

						next.t->RotationOffset = current.t->GlobalRotation;
						next.t->PositionOffset = current.t->GlobalPosition;
						next.t->ScaleOffset = current.t->GlobalScale;
						current = next;

						continue;
					}
				}
			}			

			current.t = nullptr;

			if (stack.size() > 0) {
				current = stack[stack.size() - 1];
				stack.pop_back();
			}
		}
		

		/*GlobalRotation = Rotation + RotationOffset;
		GlobalPosition = Position + PositionOffset;
		GlobalScale = Scale;
		if (UseParentScale)
			GlobalScale = GlobalScale + ScaleOffset;

		ModelMatrix = glm::mat4(1);

		ModelMatrix = glm::translate(ModelMatrix, PositionOffset.ToGLM());

		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(RotationOffset.X), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(RotationOffset.Y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(RotationOffset.Z), glm::vec3(0.0f, 0.0f, 1.0f));

		GlobalPosition = Vector3(ModelMatrix * Vector4(Position.X, Position.Y, Position.Z, 1.0f).ToGLM());

		ModelMatrix = glm::translate(ModelMatrix, Position.ToGLM());
		ModelMatrix = glm::scale(ModelMatrix, GlobalScale.ToGLM());

		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.X), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.Y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.Z), glm::vec3(0.0f, 0.0f, 1.0f));

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
		}*/
	}


	void Transform::AttachTo(Transform* baseTransform) {
		for (auto* t : ChildTransforms) {
			if (t == baseTransform) {
				Logger::Log(LogMessageLevels::WARNING, "este transform no puede atarse a su hijo.");

				return;
			}
		}
		
		ParentTransform = baseTransform;

		baseTransform->ChildTransforms.push_back(this);
		baseTransform->isParent = true;

		RotationOffset = ParentTransform->Rotation;
		PositionOffset = ParentTransform->Position;
		ScaleOffset = ParentTransform->Scale;

		//if (!isSkeletal)
			//UpdateModel();

		isAttached = true;
	}


	void Transform::UnAttach() {
		ParentTransform = nullptr;

		RotationOffset = Vector3(0.0f);

		isAttached = false;
	}


	void Transform::toGlmMat(const aiMatrix4x4* src, glm::mat4& dst) {
		dst[0][0] = src->a1; dst[1][0] = src->a2;
		dst[2][0] = src->a3; dst[3][0] = src->a4;
		dst[0][1] = src->b1; dst[1][1] = src->b2;
		dst[2][1] = src->b3; dst[3][1] = src->b4;
		dst[0][2] = src->c1; dst[1][2] = src->c2;
		dst[2][2] = src->c3; dst[3][2] = src->c4;
		dst[0][3] = src->d1; dst[1][3] = src->d2;
		dst[2][3] = src->d3; dst[3][3] = src->d4;
	}

}