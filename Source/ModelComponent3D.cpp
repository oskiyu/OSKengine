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
using namespace OSK::PERSISTENCE;

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
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::ModelComponent3D>(const OSK::ECS::ModelComponent3D& data) {
	nlohmann::json output{};

	output["m_model"] = data.m_model->GetAssetFilename();
	output["m_castShadows"] = data.m_castShadows;
	// output["m_animator"] = SerializeJson<Animator>(data.m_animator);

	for (const auto& shaderName : data.m_shaderNames) {
		output["m_shaderNames"].push_back(shaderName);
	}

	return output;
}

template <>
OSK::ECS::ModelComponent3D PERSISTENCE::DeserializeComponent<OSK::ECS::ModelComponent3D>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	ModelComponent3D output{};

	output.m_castShadows = json["m_castShadows"];
	output.m_model = Engine::GetAssetManager()->Load<ASSETS::Model3D>(json["m_model"]);
	// output.m_animator = DeserializeJson<Animator>(json["m_animator"]);

	if (json.contains("m_shaderNames")) {
		for (const std::string& shaderName : json["m_shaderNames"]) {
			output.m_shaderNames.insert(shaderName);
		}
	}

	return output;
}



template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::ModelComponent3D>(const OSK::ECS::ModelComponent3D& data) {
	BinaryBlock output{};

	output.WriteString(data.m_model->GetAssetFilename());

	output.Write<TByte>(data.m_castShadows);
	// output["m_animator"] = SerializeJson<Animator>(data.m_animator);

	output.Write<USize64>(data.m_shaderNames.size());
	for (const auto& shaderName : data.m_shaderNames) {
		output.WriteString(shaderName);
	}

	return output;
}

template <>
OSK::ECS::ModelComponent3D PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::ModelComponent3D>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	ModelComponent3D output{};

	output.m_model = Engine::GetAssetManager()->Load<ASSETS::Model3D>(reader->ReadString());
	output.m_castShadows = reader->Read<TByte>();
	// output.m_animator = DeserializeJson<Animator>(json["m_animator"]);

	const USize64 numShaderPasses = reader->Read<USize64>();
	for (UIndex64 i = 0; i < numShaderPasses; i++) {
		output.m_shaderNames.insert(reader->ReadString());
	}

	return output;
}
