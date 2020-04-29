#include "Model.h"

#include <gtc/matrix_transform.hpp>

namespace OSK {

	Model::Model(const Vector3& position, const Vector3& size, const Vector3& rotation) {
		ModelTransform = Transform();

		ModelTransform.UseModelMatrix = true;

		ModelTransform.Position = position;
		ModelTransform.Scale = size;
		ModelTransform.Rotation = rotation;

		ModelTransform.UpdateModel();
	}


	Model::~Model() {

	}


	void Model::SetPosition(const OSK::Vector3& position) {
		ModelTransform.SetPosition(position);
	}


	void Model::SetScale(const OSK::Vector3& size) {
		ModelTransform.SetScale(size);
	}


	void Model::SetRotation(const OSK::Vector3& rotation) {
		ModelTransform.SetRotation(rotation);
	}

}