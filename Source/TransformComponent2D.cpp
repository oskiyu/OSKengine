#include "TransformComponent2D.h"
#include <glm/ext/matrix_transform.hpp>
#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "SavedGameObjectTranslator.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::PERSISTENCE;


Transform2D::Transform2D(ECS::GameObjectIndex owner) : owner(owner) {
	localPosition = Vector2f::Zero;
	localScale = Vector2f::One;
	localRotation = 0;

	globalPosition = Vector2f::Zero;
	globalScale = Vector2f::Zero;
	globalRotation = 0;

	positionOffset = Vector2f::Zero;
	scaleOffset = Vector2f::Zero;
	rotationOffset = 0;

	UpdateModel();
}

Transform2D::Transform2D(ECS::GameObjectIndex owner, const Vector2f& position, const Vector2f& scale, float rotation) : owner(owner) {
	this->localPosition = position;
	this->localScale = scale;
	this->localRotation = rotation;

	globalPosition = Vector2f::Zero;
	globalScale = Vector2f::Zero;
	globalRotation = 0.0f;

	positionOffset = Vector2f::Zero;
	scaleOffset = Vector2f::Zero;
	rotationOffset = 0.0f;

	UpdateModel();
}

void Transform2D::SetPosition(const Vector2f& position) {
	localPosition = position;
	UpdateModel();
}

void Transform2D::SetScale(const Vector2f& scale) {
	localScale = scale;
	UpdateModel();
}

void Transform2D::SetRotation(float rotation) {
	localRotation = rotation;
	UpdateModel();
}

void Transform2D::AddPosition(const Vector2f& positionDelta) {
	SetPosition(localPosition + positionDelta);
}

void Transform2D::AddScale(const Vector2f& scaleDelta) {
	SetScale(localScale + scaleDelta);
}

void Transform2D::AddRotation(float rotationDelta) {
	SetRotation(localRotation + rotationDelta);
}

void Transform2D::UpdateModel() {
	globalPosition = localPosition + positionOffset;
	globalRotation = localRotation + rotationOffset;
	globalScale = localScale + scaleOffset;

	modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::translate(modelMatrix, glm::vec3(globalPosition.x, globalPosition.y, 0.0f));

	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f * globalScale.x, 0.5f * globalScale.y, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(globalRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f * globalScale.x, -0.5f * globalScale.y, 0.0f));

	modelMatrix = glm::scale(modelMatrix, glm::vec3(globalScale.x, globalScale.y, 1.0f));

	DynamicArray<GameObjectIndex> childrenToRemove;
	for (UIndex32 i = 0; i < childTransforms.GetSize(); i++) {
		Transform2D& child = Engine::GetEcs()->GetComponent<Transform2D>(childTransforms.At(i));
		if (child.parent == owner) {
			child.rotationOffset = globalRotation;
			child.positionOffset = globalPosition;
			child.scaleOffset = globalScale;
			child.UpdateModel();
		}
		else {
			childrenToRemove.Push(child.owner);
		}
	}

	for (const auto& i : childrenToRemove)
		childTransforms.Remove(i);
}

void Transform2D::AttachToObject(GameObjectIndex baseTransform) {
	for (auto c : childTransforms)
		if (baseTransform == c)
			return;

	parent = baseTransform;
	Engine::GetEcs()->GetComponent<Transform2D>(baseTransform).childTransforms.Insert(owner);
}

void Transform2D::UnAttach() {
	if (!owner.IsEmpty() && !parent.IsEmpty())
		Engine::GetEcs()->GetComponent<Transform2D>(parent).childTransforms.Remove(owner);

	owner = EMPTY_GAME_OBJECT;
}

Vector4 Transform2D::GetRectangle() const {
	return Vector4(globalPosition.x, globalPosition.y, globalScale.x, globalScale.y);
}

Vector2f Transform2D::GetPosition() const {
	return globalPosition;
}

Vector2f Transform2D::GetScale() const {
	return globalScale;
}

float Transform2D::GetRotation() const {
	return globalRotation;
}

Vector2f Transform2D::GetLocalPosition() const {
	return localPosition;
}

Vector2f Transform2D::GetLocalScale() const {
	return localScale;
}

float Transform2D::GetLocalRotation() const {
	return localRotation;
}

glm::mat4 Transform2D::GetAsMatrix() const {
	return modelMatrix;
}

GameObjectIndex Transform2D::GetParentObject() const {
	return parent;
}

Vector2f Transform2D::GetPositionOffset() const {
	return positionOffset;
}

Vector2f Transform2D::GetScaleOffset() const {
	return scaleOffset;
}

float Transform2D::GetRotationOffset() const {
	return rotationOffset;
}

template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::Transform2D>(const OSK::ECS::Transform2D& data) {
	nlohmann::json output{};

	output["parentId"] = data.GetParentObject().Get();
	output["ownerId"] = data.owner.Get();

	for (const auto& childId : data.childTransforms) {
		output["childIds"].push_back(childId.Get());
	}

	output["position"]["x"] = data.GetPosition().x;
	output["position"]["y"] = data.GetPosition().y;

	output["scale"]["x"] = data.GetScale().x;
	output["scale"]["y"] = data.GetScale().y;

	output["rotation"] = data.GetRotation();

	return output;
}

template <>
OSK::ECS::Transform2D PERSISTENCE::DeserializeComponent<OSK::ECS::Transform2D>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	Transform2D output = Transform2D(GameObjectIndex(json["ownerId"]));

	output.parent = gameObjectTranslator.GetCurrentIndex(GameObjectIndex(json["parentId"]));

	output.globalPosition.x = json["position"]["x"];
	output.globalPosition.y = json["position"]["y"];

	output.globalScale.x = json["scale"]["x"];
	output.globalScale.y = json["scale"]["y"];

	output.globalRotation = json["rotation"];

	if (json.contains("childIds")) {
		for (const auto& child : json["childIds"]) {
			output.childTransforms.Insert(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(child)));
		}
	}

	return output;
}


template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::Transform2D>(const OSK::ECS::Transform2D& data) {
	BinaryBlock output{};

	output.Write<GameObjectIndex::TUnderlyingType>(data.owner.Get());
	output.Write<GameObjectIndex::TUnderlyingType>(data.GetParentObject().Get());

	output.AppendBlock(SerializeBinaryVector2<Vector2f>(data.GetPosition()));
	output.AppendBlock(SerializeBinaryVector2<Vector2f>(data.GetScale()));
	output.Write<float>(data.GetRotation());

	output.Write<USize64>(data.childTransforms.GetSize());
	for (const auto& childId : data.childTransforms) {
		output.Write<GameObjectIndex::TUnderlyingType>(childId.Get());
	}

	return output;
}

template <>
OSK::ECS::Transform2D PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::Transform2D>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	Transform2D output = Transform2D(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>()));

	output.parent = gameObjectTranslator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>()));

	output.globalPosition = DeserializeBinaryVector2<Vector2f, float>(reader);
	output.globalScale = DeserializeBinaryVector2<Vector2f, float>(reader);
	output.globalRotation = reader->Read<float>();

	const auto numChilds = reader->Read<USize64>();
	for (UIndex64 i = 0; i < numChilds; i++) {
		output.childTransforms.Insert(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>())));
	}

	return output;
}
