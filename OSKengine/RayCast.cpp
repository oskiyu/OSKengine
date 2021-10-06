#include "RayCast.h"

using namespace OSK;

RayCastCollisionInfo RayCast::CastRay(const Vector3f& origin, Vector3f direction, Collision::SAT_Collider sat) {
	RayCastCollisionInfo info;
	info.isColliding = false;

	float min = std::numeric_limits<float>::min();
	float max = std::numeric_limits<float>::max();

	const Vector3f aabb_min = Vector3f{ -1.0f } *sat.GetTransform()->GetScale();
	const Vector3f aabb_max = Vector3f{ 1.0f } *sat.GetTransform()->GetScale();

	Vector3f OBBposition_worldspace(sat.GetTransform()->AsMatrix()[3].x, sat.GetTransform()->AsMatrix()[3].y, sat.GetTransform()->AsMatrix()[3].z);
	Vector3f delta = OBBposition_worldspace - origin;
	{
		Vector3f xaxis(sat.GetTransform()->AsMatrix()[0].x, sat.GetTransform()->AsMatrix()[0].y, sat.GetTransform()->AsMatrix()[0].z);
		float e = xaxis.Dot(delta);
		float f = direction.Dot(xaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.X) / f;
			float t2 = (e + aabb_max.X) / f;

			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < max)
				max = t2;
			if (t1 > min)
				min = t1;

			if (max < min)
				goto return_p;

		}
		else {
			if (-e + aabb_min.X > 0.0f || -e + aabb_max.X < 0.0f)
				goto return_p;
		}
	}

	{
		Vector3f yaxis(sat.GetTransform()->AsMatrix()[1].x, sat.GetTransform()->AsMatrix()[1].y, sat.GetTransform()->AsMatrix()[1].z);
		float e = yaxis.Dot(delta);
		float f = direction.Dot(yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.Y) / f;
			float t2 = (e + aabb_max.Y) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < max)
				max = t2;
			if (t1 > min)
				min = t1;
			if (min > max)
				goto return_p;

		}
		else {
			if (-e + aabb_min.Y > 0.0f || -e + aabb_max.Y < 0.0f)
				goto return_p;
		}
	}

	{
		Vector3f zaxis(sat.GetTransform()->AsMatrix()[2].x, sat.GetTransform()->AsMatrix()[2].y, sat.GetTransform()->AsMatrix()[2].z);
		float e = zaxis.Dot(delta);
		float f = direction.Dot(zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.Z) / f;
			float t2 = (e + aabb_max.Z) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < max)
				max = t2;
			if (t1 > min)
				min = t1;
			if (min > max)
				goto return_p;

		}
		else {
			if (-e + aabb_min.Z > 0.0f || -e + aabb_max.Z < 0.0f)
				goto return_p;
		}
	}

	info.isColliding = max >= min;
	info.distanceFromOrigin = min;

return_p:

	return info;
}
