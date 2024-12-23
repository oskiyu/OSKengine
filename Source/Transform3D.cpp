#include "Transform3D.h"

#include "MatrixOperations.hpp"


void OSK::Transform3D::SetPosition(const Vector3f& position) {
	AddPosition(position - m_localPosition);
}

void OSK::Transform3D::SetScale(const Vector3f& scale) {
	AddScale(scale - m_localScale);
}

void OSK::Transform3D::SetRotation(const Quaternion& rotation) {
	ApplyRotation(rotation - m_localRotation);
}

void OSK::Transform3D::AddPosition(const Vector3f& positionDelta) {
	m_changeInPosition += positionDelta;
	m_isPositionDirty.atomic = true;
}

void OSK::Transform3D::AddPosition_ThreadSafe(const Vector3f& positionDelta) {
	std::lock_guard lock(m_positionMutex.mutex);
	m_changeInPosition += positionDelta;
	m_isPositionDirty.atomic = true;
}

void OSK::Transform3D::AddScale(const Vector3f& scaleDelta) {
	m_changeInScale += scaleDelta;
	m_isScaleDirty.atomic = true;
}

void OSK::Transform3D::AddScale_ThreadSafe(const Vector3f& scaleDelta) {
	std::lock_guard lock(m_scaleMutex.mutex);
	m_changeInScale += scaleDelta;
	m_isScaleDirty.atomic = true;
}

void OSK::Transform3D::ApplyRotation(const Quaternion& rotationDelta) {
	m_changeInRotation += rotationDelta;
	m_isRotationDirty.atomic = true;
}

void OSK::Transform3D::ApplyRotation_ThreadSafe(const Quaternion& rotationDelta) {
	std::lock_guard lock(m_rotationMutex.mutex);
	m_changeInRotation += rotationDelta;
	m_isRotationDirty.atomic = true;
}

void OSK::Transform3D::RotateLocalSpace(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_LocalSpace(angle, axis);
	const Quaternion difference = copy - m_localRotation;

	ApplyRotation(difference);
}

void OSK::Transform3D::RotateLocalSpace_ThreadSafe(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_LocalSpace(angle, axis);
	const Quaternion difference = copy - m_localRotation;

	ApplyRotation_ThreadSafe(difference);
}

void OSK::Transform3D::RotateWorldSpace(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_WorldSpace(angle, axis);
	const Quaternion difference = m_localRotation - copy;

	ApplyRotation(difference);
}

void OSK::Transform3D::RotateWorldSpace_ThreadSafe(float angle, const Vector3f& axis) {
	auto copy = Quaternion(m_localRotation);
	copy.Rotate_WorldSpace(angle, axis);
	const Quaternion difference = m_localRotation - copy;

	ApplyRotation_ThreadSafe(difference);
}

OSK::Vector3f OSK::Transform3D::TransformPoint(const Vector3f& point) const {
	return Math::TransformPoint(point, m_matrix);
}

OSK::Vector3f OSK::Transform3D::GetPosition() const {
	return m_position;
}

OSK::Vector3f OSK::Transform3D::GetScale() const {
	return m_scale;
}

OSK::Quaternion OSK::Transform3D::GetRotation() const {
	return Quaternion::FromGlm(m_totalRotation);
}

OSK::Vector3f OSK::Transform3D::GetLocalPosition() const {
	return m_localPosition;
}

OSK::Vector3f OSK::Transform3D::GetLocalScale() const {
	return m_localScale;
}

OSK::Quaternion OSK::Transform3D::GetLocalRotation() const {
	return m_localRotation;
}

glm::mat4 OSK::Transform3D::GetAsMatrix() const {
	return m_matrix;
}

OSK::Vector3f OSK::Transform3D::GetForwardVector() const {
	return GetRotation().RotateVector(Vector3f(0, 0, 1));
}

OSK::Vector3f OSK::Transform3D::GetRightVector() const {
	return GetRotation().RotateVector(Vector3f(-1, 0, 0));
}

OSK::Vector3f OSK::Transform3D::GetTopVector() const {
	return GetRotation().RotateVector(Vector3f(0, 1, 0));
}

void OSK::Transform3D::OverrideMatrix(const glm::mat4& matrix) {
	m_matrix = matrix;
}

void OSK::Transform3D::UpdateModel(const Transform3D* parent, const InheritanceConfig& config) {
	m_matrix = glm::mat4(1.0f);

	if (parent) {
		m_totalRotation = config.inheritsRotation
			? parent->m_totalRotation * m_localRotation.ToMat4()
			: m_localRotation.ToMat4();

		if (config.inheritsPosition) {
			m_matrix = glm::translate(m_matrix, parent->GetPosition().ToGlm());
		}

		if (config.inheritsRotation) {
			m_matrix = m_matrix * parent->m_totalRotation;
		}
	}
	else {
		m_totalRotation = m_localRotation.ToMat4();
	}

	// Posición local.
	m_matrix = glm::translate(m_matrix, m_localPosition.ToGlm());

	// Rotación local.
	m_matrix = m_matrix * m_localRotation.ToMat4();

	// Escala local.
	m_matrix = glm::scale(m_matrix, m_localScale.ToGlm());


	// Obtener posición final.
	m_position = Vector3f(m_matrix * glm::vec4(0, 0, 0, 1));
}

void OSK::Transform3D::_ApplyChanges() {
	_ApplyChanges(nullptr, {});
}

void OSK::Transform3D::_ApplyChanges(const Transform3D* parent, const InheritanceConfig& config) {
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
		UpdateModel(parent, config);
	}
}
