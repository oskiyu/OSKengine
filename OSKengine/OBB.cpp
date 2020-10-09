#include "OBB.h"

using namespace OSK;
using namespace OSK::Collision;

OBB_Projection3D OBB::ProjectToAxis(const Vector3f& axis) const {
	const std::array<Vector3f, 8> points = GetPoints();

	float min = axis.Dot(points[0]);
	float max = min;

	for (const auto& i : points) {
		float proj = axis.Dot(i);

		if (proj < min)
			min = proj;

		if (proj > max)
			max = proj;
	}

	return { min, max };
}

bool OBB::Intersects(const OBB& obb) const {
	const std::array<Vector3f, 3> axes1 = GetAxes();
	const std::array<Vector3f, 3> axes2 = obb.GetAxes();

	for (const auto& axis : axes1) {
		Collision::OBB_Projection3D proj1 = ProjectToAxis(axis);
		Collision::OBB_Projection3D proj2 = obb.ProjectToAxis(axis);

		if (!Collision::porjectionOverlaps(proj1, proj2))
			return false;
	}

	for (const auto& axis : axes2) {
		Collision::OBB_Projection3D proj1 = ProjectToAxis(axis);
		Collision::OBB_Projection3D proj2 = obb.ProjectToAxis(axis);

		if (!Collision::porjectionOverlaps(proj1, proj2))
			return false;
	}

	return true;
}

std::array<Vector3f, 3> OBB::GetAxes() const {
	std::array<Vector3f, 3> axis = {};

	std::array<Vector3f, 8> points = GetPoints();

	Vector3f vec1 = points[3] - points[1];
	Vector3f vec2 = points[0] - points[1];

	axis[0] = vec2.Cross(vec1).GetNormalized();

	vec1 = points[7] - points[3];
	vec2 = points[6] - points[7];

	axis[1] = vec1.Cross(vec2).GetNormalized();

	vec1 = points[6] - points[4];
	vec2 = points[0] - points[4];

	axis[2] = vec1.Cross(vec2).GetNormalized();

	return axis;
}

std::array<Vector3f, 8> OBB::GetPoints() const {
	std::array<Vector3f, 8> points = {};

	glm::mat4 initMat = glm::mat4(1.0f);
	initMat = glm::translate(initMat, BoxTransform.GlobalPosition.ToGLM());
	initMat = glm::rotate(initMat, glm::radians(BoxTransform.GlobalRotation.X), { 1.0f, 0.0f, 0.0f });
	initMat = glm::rotate(initMat, glm::radians(BoxTransform.GlobalRotation.Y), { 0.0f, 1.0f, 0.0f });
	initMat = glm::rotate(initMat, glm::radians(BoxTransform.GlobalRotation.Z), { 0.0f, 0.0f, 1.0f });
	initMat = glm::scale(initMat, BoxTransform.GlobalScale.ToGLM());

	const glm::vec4 positionMult = { 0.0f, 0.0f, 0.0f, 1.0f };

	glm::mat4 mat = glm::translate(initMat, { 1, 1, 1 });
	points[0] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { 1, 1, -1 });
	points[1] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { 1, -1, 1 });
	points[2] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { 1, -1, -1 });
	points[3] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { -1, 1, 1 });
	points[4] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { -1, 1, -1 });
	points[5] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { -1, -1, 1 });
	points[6] = Vector3f(mat * positionMult);

	mat = glm::translate(initMat, { -1, -1, -1 });
	points[7] = Vector3f(mat * positionMult);

	return points;
}