#include "SAT_Collider.h"
#include "ToString.h"
#include "Math.h"

#include <array>

using namespace OSK;
using namespace OSK::Collision;

Transform& SAT_Collider::GetTransform() {
	return trasform;
}

void SAT_Collider::OptimizeFaces() {
	std::vector<SAT_Face> faces = {};

	for (const auto& i : faces) {
		bool exists = false;

		for (const auto& f : faces) {
			const Vector3f axis1 = GetAxisFromFace(i);
			const Vector3f axis2 = GetAxisFromFace(f);
			const bool isEqual = GetAxisFromFace(i) == GetAxisFromFace(f) || GetAxisFromFace(i) == -GetAxisFromFace(f);
			if (isEqual) {
				exists = true;
				break;
			}
		}

		if (!exists)
			faces.push_back(i);
	}

	faces.clear();
	faces = faces;
}

void SAT_Collider::AddFace(const Vector3f points[], uint32_t size) {
	faces.push_back({});
	SAT_Face* face = &faces[faces.size() - 1];

	for (uint32_t i = 0; i < size; i++) {
		Vector3f point = points[i];

		int32_t iterator = -1;
		for (int32_t it = 0; it < transformedPoints.size(); it++) {
			if (transformedPoints[it] == point) {
				iterator = it;
				break;
			}
		}

		if (iterator >= 0) {
			face->push_back((uint32_t)iterator);
		}
		else {
			this->points.push_back(point);
			transformedPoints.push_back(point);
			face->push_back(transformedPoints.size() - 1);
		}
	}
}

void SAT_Collider::TransformPoints() {
	if (isStatic)
		return;

	const static glm::vec4 positionMult = { 0.0f, 0.0f, 0.0f, 1.0f };

	for (uint32_t i = 0; i < points.size(); i++) {
		glm::mat4 mat = glm::translate(trasform.AsMatrix(), points[i].ToGLM());
		transformedPoints[i] = Vector3f(mat * positionMult);
	}
}

bool SAT_Collider::Intersects(const SAT_Collider& other) const {
	std::vector<Vector3f> axes1 = GetAxes();
	std::vector<Vector3f> axes2 = other.GetAxes();

	for (const auto& axis : axes1) {
		SAT_Projection proj1 = ProjectToAxis(axis);
		SAT_Projection proj2 = other.ProjectToAxis(axis);

		if (!proj1.Overlaps(proj2))
			return false;
	}

	for (const auto& axis : axes2) {
		SAT_Projection proj1 = ProjectToAxis(axis);
		SAT_Projection proj2 = other.ProjectToAxis(axis);

		if (!proj1.Overlaps(proj2))
			return false;
	}

	return true;
}

SAT_CollisionInfo SAT_Collider::GetCollisionInfo(const SAT_Collider& other) const {
	std::vector<Vector3f> axes1 = GetAxes();
	std::vector<Vector3f> axes2 = other.GetAxes();

	Vector3f smallestAxis = { 0.0f };
	float currentOverlap = std::numeric_limits<float>::max();

	for (const auto& axis : axes1) {
		SAT_Projection proj1 = ProjectToAxis(axis);
		SAT_Projection proj2 = other.ProjectToAxis(axis);

		if (!proj1.Overlaps(proj2)) {
			return { false, { 0.0f } };
		}
		else {
			const float overlapping = proj1.GetOverlap(proj2);
			if (std::abs(overlapping) < std::abs(currentOverlap)) {
				currentOverlap = overlapping;
				smallestAxis = axis;
			}
		}
	}

	for (const auto& axis : axes2) {
		SAT_Projection proj1 = ProjectToAxis(axis);
		SAT_Projection proj2 = other.ProjectToAxis(axis);

		if (!proj1.Overlaps(proj2)) {
			return { false, {0.0f} };
		}
		else {
			const float overlapping = proj1.GetOverlap(proj2);
			if (std::abs(overlapping) < std::abs(currentOverlap)) {
				currentOverlap = overlapping;
				smallestAxis = axis;
			}
		}
	}

	const Vector3f MTV = smallestAxis.GetNormalized() * currentOverlap;

#pragma region Cálculo de puntos de colisión.

	const Vector3f projectionToOther = -MTV.GetNormalized();
	const Vector3f projectionToThis = MTV.GetNormalized();
	Vector3f pointA;
	Vector3f pointB;

	std::vector<Vector3f> pointsOther = {};
	std::vector<Vector3f> pointsThis = {};

	float currentProjectionOther = std::numeric_limits<float>::min();
	float currentProjectionThis = std::numeric_limits<float>::min();

	for (const auto& i : other.GetPoints()) {
		const Vector3f p = i - other.trasform.GetPosition();
		float proj = p.Dot(projectionToOther);

		if (CompareFloats(proj, currentProjectionOther)) {
			pointsOther.push_back(p);
		}
		else if (proj > currentProjectionOther) {
			pointsOther.clear();
			pointsOther.push_back(p);
			currentProjectionOther = proj;
		}
	}

	for (const auto& i : GetPoints()) {
		const Vector3f p = i - trasform.GetPosition();
		float proj = p.Dot(projectionToThis);

		if (CompareFloats(proj, currentProjectionThis)) {
			pointsThis.push_back(p);
		}
		else if (proj > currentProjectionThis) {
			pointsThis.clear();
			pointsThis.push_back(p);
			currentProjectionThis = proj;
		}
	}

	if (pointsThis.size() == 1) {
		pointA = pointsThis[0];
		pointB = pointA + trasform.GetPosition() - other.trasform.GetPosition();
	}
	else if (pointsOther.size() == 1) {
		pointB = pointsOther[0];
		pointA = pointB + other.trasform.GetPosition() - trasform.GetPosition();
	}
	else {
		if (pointsThis.size() < pointsOther.size()) {
			Vector3f averageThis = { 0.0f };
			for (const auto& i : pointsThis)
				averageThis += i / pointsThis.size();

			pointA = averageThis;

			pointB = pointA + trasform.GetPosition() - other.trasform.GetPosition();
		}
		else  if (pointsThis.size() > pointsOther.size()) {
			Vector3f averageOther = { 0.0f };
			for (const auto& i : pointsOther)
				averageOther += i / pointsOther.size();

			pointB = averageOther;
		
			pointA = pointB + other.trasform.GetPosition() - trasform.GetPosition();
		}
		else {
			Vector3f average = { 0.0f };
			const int number = pointsThis.size() + pointsOther.size();

			for (const auto& i : pointsThis)
				average += i / number;

			for (const auto& i : pointsOther) {
				Vector3f aPoint = i + other.trasform.GetPosition() - trasform.GetPosition();
				average += aPoint / number;
			}

			pointA = average;
			pointB = pointA + trasform.GetPosition() - other.trasform.GetPosition();
		}

	}

#pragma endregion

	SAT_CollisionInfo output{};
	output.isColliding = true;
	output.axis = smallestAxis;
	output.minimunTranslationVector = smallestAxis.GetNormalized() * currentOverlap;
	output.pointA = pointA;
	output.pointB = pointB;

	return output;
}

std::vector<Vector3f> SAT_Collider::GetPoints() const {
	return transformedPoints;
}

std::vector<Vector3f> SAT_Collider::GetAxes() const {
	std::vector<Vector3f> axes = {};
	axes.reserve(faces.size());

	for (const auto& i : faces) {
		axes.push_back(GetAxisFromFace(i));
	}

	return axes;
}

SAT_Projection SAT_Collider::ProjectToAxis(const Vector3f& axis) const {
	float min = axis.Dot(transformedPoints[0]);
	float max = min;

	for (const auto& i : transformedPoints) {
		float proj = axis.Dot(i);

		if (proj < min)
			min = proj;

		if (proj > max)
			max = proj;
	}

	return SAT_Projection(min, max);
}

Vector3f SAT_Collider::GetAxisFromFace(const SAT_Face& face) const {
	if (face.size() < 3)
		throw std::runtime_error("ERROR: SAT_Face has only " + std::to_string(points.size()) + " points.");


	const Vector3f vec1 = transformedPoints[face[1]] - transformedPoints[face[0]];
	const Vector3f vec2 = transformedPoints[face[2]] - transformedPoints[face[0]];

	return vec1.Cross(vec2).GetNormalized();
}

//
inline const std::array<Vector3f, 4> boxFaceA_points = {
	Vector3f(1, -1, -1),
	Vector3f(1, 1, -1),
	Vector3f(1, 1, 1),
	Vector3f(1, -1, 1)
}; 
inline const std::array<Vector3f, 4> boxFaceB_points = {
	Vector3f(1, -1, -1),
	Vector3f(-1, -1, -1),
	Vector3f(-1, -1, 1),
	Vector3f(1, -1, 1)
}; 
inline const std::array<Vector3f, 4> boxFaceC_points = {
	Vector3f(1, 1, 1),
	Vector3f(-1, 1, 1),
	Vector3f(-1, -1, 1),
	Vector3f(1, -1, 1)
}; 
inline const std::array<Vector3f, 4> boxFaceD_points = {
	Vector3f(1, 1, -1),
	Vector3f(1, 1, 1),
	Vector3f(-1, 1, 1),
	Vector3f(-1, 1, -1)
}; 
inline const std::array<Vector3f, 4> boxFaceE_points = {
	Vector3f(1, 1, -1),
	Vector3f(1, -1, -1),
	Vector3f(-1, -1, -1),
	Vector3f(-1, -1, -1)
}; 
inline const std::array<Vector3f, 4> boxFaceF_points = {
	Vector3f(-1, 1, 1),
	Vector3f(-1, -1, 1),
	Vector3f(-1, -1, -1),
	Vector3f(-1, 1, -1)
};

SAT_Collider SAT_Collider::CreateOBB() {
	SAT_Collider collider{};

	collider.AddFace(boxFaceA_points.data(), boxFaceA_points.size());
	collider.AddFace(boxFaceB_points.data(), boxFaceB_points.size());
	collider.AddFace(boxFaceC_points.data(), boxFaceC_points.size());
	collider.AddFace(boxFaceD_points.data(), boxFaceD_points.size());
	collider.AddFace(boxFaceE_points.data(), boxFaceE_points.size());
	collider.AddFace(boxFaceF_points.data(), boxFaceF_points.size());

	collider.OptimizeFaces();

	return collider;
}