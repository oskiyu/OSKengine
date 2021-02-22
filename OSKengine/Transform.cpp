#include "Transform.h"

#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>
#include <gtx/string_cast.hpp>

#include "ToString.h"

using namespace OSK;

Transform::Transform() {
		ModelMatrix = glm::mat4(1);

		LocalPosition = Vector3f(0);
		LocalScale = Vector3f(1);

		GlobalPosition = Vector3f(0);
		GlobalScale = Vector3f(1);

		isAttached = false;
		ParentTransform = nullptr;
	}

Transform::Transform(const Vector3f& position, const Vector3f& scale, const Vector3f& rotation) {
		ModelMatrix = glm::mat4(1.0f);
		LocalPosition = position;
		LocalScale = scale;

		isAttached = false;
		ParentTransform = nullptr;

		UpdateModel();
	}
	
Transform::~Transform() {

	}

void Transform::SetPosition(const Vector3f& position) {
		LocalPosition = position;

		UpdateModel();
	}

void Transform::SetScale(const Vector3f& scale) {
		LocalScale = scale;

		UpdateModel();
	}

void Transform::SetRotation(const Quaternion& rotation) {
		Rotation = rotation;
		
		UpdateModel();
	}
	
void Transform::AddPosition(const Vector3f& positionDelta) {
		SetPosition(LocalPosition + positionDelta);
	}

void Transform::AddScale(const Vector3f& scaleDelta) {
		SetScale(LocalScale + scaleDelta);
	}

void Transform::ApplyRotation(const Quaternion& rotationDelta) {
		Quaternion q = Rotation;
		q.Quat *= rotationDelta.Quat;

		SetRotation(q);
	}

void Transform::RotateLocalSpace(float angle, const Vector3f& axis) {
		Rotation.Rotate_LocalSpace(angle, axis);

		UpdateModel();
	}

void Transform::RotateWorldSpace(float angle, const Vector3f& axis) {
		Rotation.Rotate_WorldSpace(angle, axis);

		UpdateModel();
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

		isAttached = true;
	}

void Transform::UnAttach() {
		ParentTransform = nullptr;

		isAttached = false;
	}

void Transform::UpdateModel() {
		ModelMatrix = glm::mat4(1.0f);
		if (ParentTransform) {
			ModelMatrix = ParentTransform->ModelMatrix;
			GlobalScale = LocalScale + ParentTransform->GlobalScale;
		}

		//Posición local.
		ModelMatrix = glm::translate(ModelMatrix, LocalPosition.ToGLM());

		//Escala local.
		ModelMatrix = glm::scale(ModelMatrix, LocalScale.ToGLM());

		//Rotación local.
		ModelMatrix = ModelMatrix * Rotation.ToMat4();

		//Obtener posición final.
		GlobalPosition = Vector3f(ModelMatrix * Vector4(0, 0, 0, 1).ToGLM());

		if (isParent) {
			for (uint32_t i = 0; i < ChildTransforms.size(); i++) {

				if (ChildTransforms[i]->ParentTransform == this)
					ChildTransforms[i]->UpdateModel();
				else
					ChildTransforms.erase(ChildTransforms.begin() + i);

			}
		}
	}

glm::mat4 Transform::toGlmMat(const aiMatrix4x4* src) {
		glm::mat4 dst;

		dst[0][0] = src->a1; dst[1][0] = src->a2;
		dst[2][0] = src->a3; dst[3][0] = src->a4;
		dst[0][1] = src->b1; dst[1][1] = src->b2;
		dst[2][1] = src->b3; dst[3][1] = src->b4;
		dst[0][2] = src->c1; dst[1][2] = src->c2;
		dst[2][2] = src->c3; dst[3][2] = src->c4;
		dst[0][3] = src->d1; dst[1][3] = src->d2;
		dst[2][3] = src->d3; dst[3][3] = src->d4;

		return dst;
	}

