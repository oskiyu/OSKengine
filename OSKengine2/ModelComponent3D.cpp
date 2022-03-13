#include "ModelComponent3D.h"

#include "MaterialInstance.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

ModelComponent3D::~ModelComponent3D() {
	if (materialInstance.HasValue())
		materialInstance.Delete();
}

void ModelComponent3D::SetModel(Model3D* model) {
	this->model = model;
}

void ModelComponent3D::SetMaterialInstance(OwnedPtr<MaterialInstance> materialInstance) {
	if (this->materialInstance.HasValue())
		delete this->materialInstance.GetPointer();

	this->materialInstance = materialInstance;
}

Model3D* ModelComponent3D::GetModel() const {
	return model;
}

MaterialInstance* ModelComponent3D::GetMaterialInstance() const {
	return materialInstance.GetPointer();
}
