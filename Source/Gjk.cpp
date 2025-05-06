#include "Gjk.h"

#include "Simplex.h"

/// @brief Dummy class para la comprobación
/// contra un punto.
class _GjkPointCollider : public OSK::COLLISION::IGjkCollider {
public:

	explicit _GjkPointCollider(const OSK::Vector3f& point) : point(point){}

	OSK::COLLISION::GjkSupport GetSupport(const OSK::Vector3f&) const override {
		return { point, 0 };
	}

	OSK::Vector3f point{};
};

OSK::COLLISION::MinkowskiSupport OSK::COLLISION::GetMinkowskiSupport(const IGjkCollider& first, const IGjkCollider& second, const Vector3f& direction) {
	const auto& firstSupport  = first .GetSupport( direction);
	const auto& secondSupport = second.GetSupport(-direction);
	
	MinkowskiSupport output{};
	output.point = firstSupport.point - secondSupport.point;

	output.originalVertexIdA = firstSupport.originalVertexId;
	output.originalVertexIdB = secondSupport.originalVertexId;

	output.worldSpacePointA = firstSupport.point;
	output.worldSpacePointB = secondSupport.point;

	return output;
}

bool OSK::COLLISION::IGjkCollider::ContainsPoint(const Vector3f& point) const {
	return Simplex::GenerateFrom(*this, _GjkPointCollider(point)).ContainsOrigin();
}

bool OSK::COLLISION::IGjkCollider::IsBehindPlane(Plane plane) const {
	// Punto más hacia atrás del plano.
	const auto supportBehindPlane = GetSupport(-plane.normal).point;

	const float supportProjection = supportBehindPlane.Dot(plane.normal);
	const float planeProjection = plane.point.Dot(plane.normal);

	return supportProjection < planeProjection;
}

bool OSK::COLLISION::IGjkCollider::IsFullyBehindPlane(Plane plane) const {
	// Punto más hacia delante del plano.
	// Si el punto está por detrás del plano,
	// entonces necesariamente todo el collider
	// está detrás del plano.
	const auto supportBehindPlane = GetSupport(plane.normal).point;

	const float supportProjection = supportBehindPlane.Dot(plane.normal);
	const float planeProjection = plane.point.Dot(plane.normal);

	return supportProjection > planeProjection;
}

bool OSK::COLLISION::IGjkCollider::IsInsideFrustum(const AnyFrustum& frustum) const {
	for (const auto& plane : frustum) {
		if (this->IsBehindPlane(plane)) {
			return false;
		}
	}

	return true;
}
