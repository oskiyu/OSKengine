#include "OldModel.h"

#include <gtc/matrix_transform.hpp>

namespace OSK {

	OldModel::OldModel(const Vector3& position, const Vector3& size, const Vector3& rotation) {
		ModelTransform = Transform();

		ModelTransform.UseModelMatrix = true;

		ModelTransform.Position = position;
		ModelTransform.Scale = size;
		ModelTransform.Rotation = rotation;

		ModelTransform.UpdateModel();
	}


	OldModel::~OldModel() {

	}


	void OldModel::SetPosition(const OSK::Vector3& position) {
		ModelTransform.SetPosition(position);
	}


	void OldModel::SetScale(const OSK::Vector3& size) {
		ModelTransform.SetScale(size);
	}


	void OldModel::SetRotation(const OSK::Vector3& rotation) {
		ModelTransform.SetRotation(rotation);
	}

}