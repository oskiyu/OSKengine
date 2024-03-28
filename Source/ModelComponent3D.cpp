#include "ModelComponent3D.h"

#include "MaterialInstance.h"
#include "Model3D.h"
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

	for (const auto& animation : model->GetModel().GetCpuModel().GetAnimations()) {
		m_animator.AddAnimation(animation);
	}

	for (const auto& animationSkin : model->GetModel().GetCpuModel().GetAnimationSkins()) {
		m_animator.AddSkin(animationSkin);
	}

	if (!model->GetModel().GetCpuModel().GetAnimationSkins().IsEmpty()) {
		m_animator.SetActiveSkin(model->GetModel().GetCpuModel().GetAnimationSkins()[0].name);
	}
}

ASSETS::Model3D* ModelComponent3D::GetModelAsset() {
	return m_model.GetAsset();
}

const ASSETS::Model3D* ModelComponent3D::GetModelAsset() const {
	return m_model.GetAsset();
}

GRAPHICS::GpuModel3D* ModelComponent3D::GetModel() {
	return &m_model->_GetModel();
}

const GRAPHICS::GpuModel3D* ModelComponent3D::GetModel() const {
	return &m_model->GetModel();
}

GRAPHICS::Animator& ModelComponent3D::GetAnimator() {
	return m_animator;
}

const GRAPHICS::Animator& ModelComponent3D::GetAnimator() const {
	return m_animator;
}

bool ModelComponent3D::IsAnimated() const {
	return m_animator.HasAnimations();
}

void ModelComponent3D::AddShaderPassName(const std::string& name) {
	m_shaderNames.insert(name);
}

void ModelComponent3D::RemoveShaderPassName(const std::string& name) {
	m_shaderNames.erase(name);
}

const std::unordered_set<std::string, StringHasher, std::equal_to<>>& ModelComponent3D::GetShaderPassNames() const {
	return m_shaderNames;
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
