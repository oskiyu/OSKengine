#include "Transform3D.h"

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


Transform3D Transform3D::FromMatrix(ECS::GameObjectIndex owner, const glm::mat4& matrix) {
	Transform3D output(owner);

	// todo

	return output;
}


Transform3D::Transform3D(ECS::GameObjectIndex owner) : m_ownerId(owner) {

}

void Transform3D::SetPosition(const Vector3f& nPosition) {
	AddPosition(nPosition - m_localPosition);
}

void Transform3D::SetScale(const Vector3f& nScale) {
	AddScale(nScale - m_localScale);

	// if (parent && m_inheritScale)
	//	m_localScale += GetEcs()->GetComponent<Transform3D>(parent).GetLocalScale();
}

void Transform3D::SetRotation(const Quaternion& nRotation) {
	ApplyRotation(nRotation - m_localRotation);
}

void Transform3D::AddPosition(const Vector3f& positionDelta) {
	m_changeInPosition += positionDelta;
	m_isPositionDirty.atomic = true;
}

void Transform3D::AddPosition_ThreadSafe(const Vector3f& positionDelta) {
	std::lock_guard lock(m_positionMutex.mutex);
	m_changeInPosition += positionDelta;
	m_isPositionDirty.atomic = true;
}

void Transform3D::AddScale(const Vector3f& scaleDelta) {
	m_changeInScale += scaleDelta;
	m_isScaleDirty.atomic = true;
}

void Transform3D::AddScale_ThreadSafe(const Vector3f& scaleDelta) {
	std::lock_guard lock(m_scaleMutex.mutex);
	m_changeInScale += scaleDelta;
	m_isScaleDirty.atomic = true;
}

void Transform3D::ApplyRotation(const Quaternion& rotationDelta) {
	m_changeInRotation += rotationDelta;
	m_isRotationDirty.atomic = true;
}

void Transform3D::ApplyRotation_ThreadSafe(const Quaternion& rotationDelta) {
	std::lock_guard lock(m_rotationMutex.mutex);
	m_changeInRotation += rotationDelta;
	m_isRotationDirty.atomic = true;
}

void Transform3D::RotateLocalSpace(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_LocalSpace(angle, axis);
	const Quaternion difference = copy - m_localRotation;

	ApplyRotation(difference);
}

void Transform3D::RotateLocalSpace_ThreadSafe(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_WorldSpace(angle, axis);
	const Quaternion difference = copy - m_localRotation;

	ApplyRotation_ThreadSafe(difference);
}

void Transform3D::RotateWorldSpace(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_WorldSpace(angle, axis);
	const Quaternion difference = m_localRotation - copy;

	ApplyRotation(difference);
}

void Transform3D::RotateWorldSpace_ThreadSafe(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_WorldSpace(angle, axis);
	const Quaternion difference = m_localRotation - copy;

	ApplyRotation_ThreadSafe(difference);
}

void Transform3D::AddChild(ECS::GameObjectIndex obj) {
	if (!m_childIds.ContainsElement(obj)) {
		m_childIds.Insert(obj);
	}
}

void Transform3D::RemoveChild(GameObjectIndex obj) {
	m_childIds.Remove(obj);
}

std::span<const GameObjectIndex> Transform3D::GetChildren() const {
	return m_childIds.GetFullSpan();
}


Vector3f Transform3D::TransformPoint(const Vector3f& point) const {
	return Math::TransformPoint(point, m_matrix);
}

Vector3f Transform3D::GetPosition() const {
	return m_globalPosition;
}

Vector3f Transform3D::GetScale() const {
	return m_globalScale;
}

Vector3f Transform3D::GetLocalPosition() const {
	return m_localPosition;
}

Vector3f Transform3D::GetLocalScale() const {
	return m_localScale;
}

Quaternion Transform3D::GetLocalRotation() const {
	return m_localRotation;
}

Quaternion Transform3D::GetRotation() const {
	return Quaternion::FromGlm(glm::toQuat(m_globalRotation));
}

glm::mat4 Transform3D::GetAsMatrix() const {
	return m_matrix;
}

void Transform3D::OverrideMatrix(const glm::mat4& matrix) {
	m_matrix = matrix;
}

Vector3f Transform3D::GetForwardVector() const {
	return GetRotation().RotateVector(Vector3f(0, 0, 1));
}

Vector3f Transform3D::GetRightVector() const {
	return GetRotation().RotateVector(Vector3f(-1, 0, 0));
}

Vector3f Transform3D::GetTopVector() const {
	return GetRotation().RotateVector(Vector3f(0, 1, 0));
}

void Transform3D::SetShouldInheritPosition(bool value) {
	m_inheritPosition = value;
}

void Transform3D::SetShouldInheritRotation(bool value) {
	m_inheritRotation = value;
}

void Transform3D::SetShouldInheritScale(bool value) {
	m_inheritScale = value;
}

void Transform3D::_ApplyChanges(std::optional<const Transform3D*> parent) {
	const bool isAnyDirty =
		m_isPositionDirty.atomic ||
		m_isRotationDirty.atomic ||
		m_isScaleDirty.atomic;

	if (m_isPositionDirty.atomic) {
		m_localPosition += m_changeInPosition;
		m_changeInPosition = Vector3f::Zero;

		m_isPositionDirty.atomic = false;
	}

	if (m_isRotationDirty.atomic) {
		m_localRotation += m_changeInRotation;
		m_changeInRotation = Quaternion::Empty();

		m_isRotationDirty.atomic = false;
	}

	if (m_isScaleDirty.atomic) {
		m_localScale += m_changeInScale;
		m_changeInScale = Vector3f::Zero;

		m_isScaleDirty.atomic = false;
	}

	if (isAnyDirty) {
		UpdateModel(parent);
	}
}

void Transform3D::UpdateModel(std::optional<const Transform3D*> parent) {
	m_matrix = glm::mat4(1.0f);

	if (parent.has_value()) {
		const Transform3D& parentTransform = *parent.value();

		m_globalRotation = m_inheritRotation
			? parentTransform.m_globalRotation * m_localRotation.ToMat4()
			: m_localRotation.ToMat4();

		if (m_inheritPosition) {
			m_matrix = glm::translate(m_matrix, parentTransform.GetPosition().ToGlm());
		}

		if (m_inheritRotation) {
			m_matrix = m_matrix * parentTransform.m_globalRotation;
		}
	}
	else {
		m_globalRotation = m_localRotation.ToMat4();
	}

	// Posici�n local.
	m_matrix = glm::translate(m_matrix, m_localPosition.ToGlm());

	// Rotaci�n local.
	m_matrix = m_matrix * m_localRotation.ToMat4();

	// Escala local.
	m_matrix = glm::scale(m_matrix, m_localScale.ToGlm());


	// Obtener posici�n final.
	m_globalPosition = Vector3f(m_matrix * glm::vec4(0, 0, 0, 1));
}


template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::Transform3D>(const OSK::ECS::Transform3D& data) {
	nlohmann::json output{};

	output["m_inheritPosition"] = data.m_inheritPosition;
	output["m_inheritRotation"] = data.m_inheritRotation;
	output["m_inheritScale"] = data.m_inheritScale;

	output["localPosition"] = SerializeVector3(data.m_localPosition);
	output["localRotation"] = SerializeData<Quaternion>(data.m_localRotation);
	output["localScale"] = SerializeVector3(data.m_localScale);

	output["m_ownerId"] = data.m_ownerId.Get();

	for (const auto& childId : data.m_childIds) {
		output["m_childIds"].push_back(childId.Get());
	}

	return output;
}

template <>
OSK::ECS::Transform3D PERSISTENCE::DeserializeComponent<OSK::ECS::Transform3D>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	auto output = Transform3D(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(json["m_ownerId"])));

	output.m_inheritPosition = json["m_inheritPosition"];
	output.m_inheritRotation = json["m_inheritRotation"];
	output.m_inheritScale = json["m_inheritScale"];


	output.m_localPosition = DeserializeVector3<Vector3f>(json["localPosition"]);
	output.m_localRotation = DeserializeData<Quaternion>(json["localRotation"]);
	output.m_localScale = DeserializeVector3<Vector3f>(json["localScale"]);


	if (json.contains("m_childIds")) {
		for (const auto& child : json["m_childIds"]) {
			output.m_childIds.Insert(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(child)));
		}
	}

	return output;
}


template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::Transform3D>(const OSK::ECS::Transform3D& data) {
	BinaryBlock output{};

	output.Write<GameObjectIndex::TUnderlyingType>(data.m_ownerId.Get());

	output.Write<TByte>(data.m_inheritPosition);
	output.Write<TByte>(data.m_inheritRotation);
	output.Write<TByte>(data.m_inheritScale);

	output.AppendBlock(SerializeBinaryVector3<Vector3f>(data.m_localPosition));
	output.AppendBlock(BinarySerializeData<Quaternion>(data.m_localRotation));
	output.AppendBlock(SerializeBinaryVector3<Vector3f>(data.m_localScale));

	output.Write<USize64>(data.m_childIds.GetSize());
	for (const auto& childId : data.m_childIds) {
		output.Write<GameObjectIndex::TUnderlyingType>(childId.Get());
	}

	return output;
}

template <>
OSK::ECS::Transform3D PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::Transform3D>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	auto output = Transform3D(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>())));

	output.m_inheritPosition = reader->Read<TByte>();
	output.m_inheritRotation = reader->Read<TByte>();
	output.m_inheritScale = reader->Read<TByte>();

	output.m_localPosition = DeserializeBinaryVector3<Vector3f, float>(reader);
	output.m_localRotation = BinaryDeserializeData<Quaternion>(reader);
	output.m_localScale = DeserializeBinaryVector3<Vector3f, float>(reader);

	const auto numChilds = reader->Read<USize64>();
	for (UIndex64 i = 0; i < numChilds; i++) {
		output.m_childIds.Insert(gameObjectTranslator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>())));
	}

	return output;
}
