#include "ModelComponent3D.h"

#include "MaterialInstance.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Material.h"
#include "IMaterialSlot.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void ModelComponent3D::SetModel(Model3D* model) {
	m_model = model;
}

Model3D* ModelComponent3D::GetModel() const {
	return m_model;
}
