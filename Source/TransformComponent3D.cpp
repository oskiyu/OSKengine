#include "TransformComponent3D.h"

#include "EntityComponentSystem.h"
#include "OSKengine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "MatrixOperations.hpp"
#include "SavedGameObjectTranslator.h"

#include "Math.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::PERSISTENCE;


TransformComponent3D TransformComponent3D::FromMatrix(ECS::GameObjectIndex owner, const glm::mat4& matrix) {
	TransformComponent3D output(owner);

	output.m_transform.OverrideMatrix(matrix);

	return output;
}


TransformComponent3D::TransformComponent3D(ECS::GameObjectIndex owner) : m_ownerId(owner) {

}

void TransformComponent3D::AddChild(ECS::GameObjectIndex obj) {
	if (!m_childIds.ContainsElement(obj)) {
		m_childIds.Insert(obj);
	}
}

void TransformComponent3D::RemoveChild(GameObjectIndex obj) {
	m_childIds.Remove(obj);
}

std::span<const GameObjectIndex> TransformComponent3D::GetChildren() const {
	return m_childIds.GetFullSpan();
}

Transform3D& TransformComponent3D::GetTransform() {
	return m_transform;
}

const Transform3D& TransformComponent3D::GetTransform() const {
	return m_transform;
}

void TransformComponent3D::SetShouldInheritPosition(bool value) {
	m_config.inheritsPosition = value;
}

void TransformComponent3D::SetShouldInheritRotation(bool value) {
	m_config.inheritsRotation = value;
}

void TransformComponent3D::SetShouldInheritScale(bool value) {
	m_config.inheritsScale = value;
}


void TransformComponent3D::_ApplyChanges(std::optional<const TransformComponent3D*> parent) {
	if (parent.has_value()) {
		m_transform._ApplyChanges(&parent.value()->GetTransform(), m_config);
	}
	else {
		m_transform._ApplyChanges(nullptr, m_config);
	}
}


template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::TransformComponent3D>(const OSK::ECS::TransformComponent3D& data) {
	nlohmann::json output{};

	output["m_inheritPosition"] = data.m_config.inheritsPosition;
	output["m_inheritRotation"] = data.m_config.inheritsRotation;
	output["m_inheritScale"] = data.m_config.inheritsScale;

	output["localPosition"] = SerializeVector3(data.m_transform.GetLocalPosition());
	output["localRotation"] = SerializeData<Quaternion>(data.m_transform.GetLocalRotation());
	output["localScale"] = SerializeVector3(data.m_transform.GetLocalScale());

	output["m_ownerId"] = data.m_ownerId.Get();

	for (const auto& childId : data.m_childIds) {
		output["m_childIds"].push_back(childId.Get());
	}

	return output;
}

template <>
OSK::ECS::TransformComponent3D PERSISTENCE::DeserializeComponent<OSK::ECS::TransformComponent3D>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	auto output = TransformComponent3D(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(json["m_ownerId"])));

	output.m_config.inheritsPosition = json["m_inheritPosition"];
	output.m_config.inheritsRotation = json["m_inheritRotation"];
	output.m_config.inheritsScale = json["m_inheritScale"];


	output.m_transform.SetPosition(DeserializeVector3<Vector3f>(json["localPosition"]));
	output.m_transform.SetRotation(DeserializeData<Quaternion>(json["localRotation"]));
	output.m_transform.SetScale(DeserializeVector3<Vector3f>(json["localScale"]));


	if (json.contains("m_childIds")) {
		for (const auto& child : json["m_childIds"]) {
			output.m_childIds.Insert(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(child)));
		}
	}

	return output;
}


template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::TransformComponent3D>(const OSK::ECS::TransformComponent3D& data) {
	BinaryBlock output{};

	output.Write<GameObjectIndex::TUnderlyingType>(data.m_ownerId.Get());

	output.Write<TByte>(data.m_config.inheritsPosition);
	output.Write<TByte>(data.m_config.inheritsRotation);
	output.Write<TByte>(data.m_config.inheritsScale);

	output.AppendBlock(SerializeBinaryVector3<Vector3f>(data.m_transform.GetLocalPosition()));
	output.AppendBlock(BinarySerializeData<Quaternion>(data.m_transform.GetLocalRotation()));
	output.AppendBlock(SerializeBinaryVector3<Vector3f>(data.m_transform.GetLocalScale()));

	output.Write<USize64>(data.m_childIds.GetSize());
	for (const auto& childId : data.m_childIds) {
		output.Write<GameObjectIndex::TUnderlyingType>(childId.Get());
	}

	return output;
}

template <>
OSK::ECS::TransformComponent3D PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::TransformComponent3D>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	auto output = TransformComponent3D(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>())));

	output.m_config.inheritsPosition = reader->Read<TByte>();
	output.m_config.inheritsRotation = reader->Read<TByte>();
	output.m_config.inheritsScale = reader->Read<TByte>();

	output.m_transform.SetPosition(DeserializeBinaryVector3<Vector3f, float>(reader));
	output.m_transform.SetRotation(BinaryDeserializeData<Quaternion>(reader));
	output.m_transform.SetScale(DeserializeBinaryVector3<Vector3f, float>(reader));

	const auto numChilds = reader->Read<USize64>();
	for (UIndex64 i = 0; i < numChilds; i++) {
		output.m_childIds.Insert(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>())));
	}

	return output;
}
