#include "Transform.h"

#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>
#include <gtx/string_cast.hpp>

#include "ToString.h"

using namespace OSK;

Transform::Transform() {
		modelMatrix = glm::mat4(1);

		localPosition = Vector3f(0);
		localScale = Vector3f(1);

		globalPosition = Vector3f(0);
		globalScale = Vector3f(1);

		isAttached = false;
		parentTransform = nullptr;
	}

Transform::Transform(const Vector3f& position, const Vector3f& scale) {
		modelMatrix = glm::mat4(1.0f);
		localPosition = position;
		localScale = scale;

		isAttached = false;
		parentTransform = nullptr;

		UpdateModel();
	}

void Transform::SetPosition(const Vector3f& position) {
		localPosition = position;

		UpdateModel();
	}

void Transform::SetScale(const Vector3f& scale) {
		localScale = scale;

		UpdateModel();
	}

void Transform::SetRotation(const Quaternion& rotation) {
		this->rotation = rotation;
		
		UpdateModel();
	}
	
void Transform::AddPosition(const Vector3f& positionDelta) {
		SetPosition(localPosition + positionDelta);
	}

void Transform::AddScale(const Vector3f& scaleDelta) {
		SetScale(localScale + scaleDelta);
	}

void Transform::ApplyRotation(const Quaternion& rotationDelta) {
		Quaternion q = rotation;
		q.Quat *= rotationDelta.Quat;

		SetRotation(q);
	}

void Transform::RotateLocalSpace(float angle, const Vector3f& axis) {
		rotation.Rotate_LocalSpace(angle, axis);

		UpdateModel();
	}

void Transform::RotateWorldSpace(float angle, const Vector3f& axis) {
		rotation.Rotate_WorldSpace(angle, axis);

		UpdateModel();
	}

void Transform::AttachTo(Transform* baseTransform) {
		for (auto* t : childTransforms) {
			if (t == baseTransform) {
				Logger::Log(LogMessageLevels::WARNING, "este transform no puede atarse a su hijo.");

				return;
			}
		}

		parentTransform = baseTransform;

		baseTransform->childTransforms.push_back(this);
		baseTransform->isParent = true;

		isAttached = true;
	}

void Transform::UnAttach() {
		parentTransform = nullptr;

		isAttached = false;
	}

void Transform::UpdateModel() {
		modelMatrix = glm::mat4(1.0f);
		if (parentTransform) {
			modelMatrix = parentTransform->modelMatrix;
			globalScale = localScale + parentTransform->globalScale;
		}

		//Posición local.
		modelMatrix = glm::translate(modelMatrix, localPosition.ToGLM());

		//Escala local.
		modelMatrix = glm::scale(modelMatrix, localScale.ToGLM());

		//Rotación local.
		modelMatrix = modelMatrix * rotation.ToMat4();

		//Obtener posición final.
		globalPosition = Vector3f(modelMatrix * Vector4(0, 0, 0, 1).ToGLM());

		if (isParent) {
			for (uint32_t i = 0; i < childTransforms.size(); i++) {

				if (childTransforms[i]->parentTransform == this)
					childTransforms[i]->UpdateModel();
				else
					childTransforms.erase(childTransforms.begin() + i);

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

Vector3f Transform::GetPosition() const {
	return globalPosition;
}

Vector3f Transform::GetScale() const {
	return globalScale;
}

Vector3f Transform::GetLocalPosition() const {
	return localPosition;
}

Vector3f Transform::GetLocalScale() const {
	return localScale;
}

Quaternion Transform::GetRotation() const {
	return rotation;
}

glm::mat4 Transform::AsMatrix() const {
	return modelMatrix;
}

Transform* Transform::GetParent() const {
	return parentTransform;
}

std::vector<Transform*> Transform::GetChildTransforms() const {
	return childTransforms;
}