#include "ModelComponent3D.h"

#include "MaterialInstance.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Material.h"
#include "IMaterialSlot.h"

#include "OSKengine.h"
#include "AssetManager.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void ModelComponent3D::SetModel(AssetRef<Model3D> model) {
	m_model = model;
}

Model3D* ModelComponent3D::GetModel() {
	return m_model.GetAsset();
}

const Model3D* ModelComponent3D::GetModel() const {
	return m_model.GetAsset();
}

template <>
nlohmann::json PERSISTENCE::SerializeJson<OSK::ECS::ModelComponent3D>(const OSK::ECS::ModelComponent3D& data) {
	nlohmann::json output{};

	output["m_model"] = data.m_model->GetAssetFilename();
	output["m_castShadows"] = data.m_castShadows;

	return output;
}

template <>
OSK::ECS::ModelComponent3D PERSISTENCE::DeserializeJson<OSK::ECS::ModelComponent3D>(const nlohmann::json& json) {
	ModelComponent3D output{};

	output.m_castShadows = json["m_castShadows"];
	output.m_model = Engine::GetAssetManager()->Load<ASSETS::Model3D>(json["m_model"]);

	return output;
}
