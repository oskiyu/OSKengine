#include "Simplex.h"

#include "NumericTypes.h"
#include "Vector3.hpp"

#include "Assert.h"
#include "InvalidObjectStateException.h"
#include "UnreachableException.h"

#include "Gjk.h"

#include <array>
#include <span>

using namespace OSK;
using namespace OSK::COLLISION;

Simplex Simplex::GenerateFrom(const IGjkCollider& first, const IGjkCollider& second) {
	Simplex simplex = {};

	// Iteración inicial.
	simplex.AddAndUpdate(GetMinkowskiSupport(first, second, Vector3f(1.0f, 0.0f, 0.0f)));
	Vector3f direction = -Vector3f(1.0f, 0.0f, 0.0f);

	// Límite de 20 iteraciones.
	constexpr static USize64 IterationLimit = 20;
	UIndex64 iteration = 0;
	for (; iteration < IterationLimit; iteration++) {
		const auto support = GetMinkowskiSupport(first, second, direction);

		if (support.point.Dot(direction) <= 0.0f) {
			return simplex;
		}

		simplex.AddAndUpdate(support);

		if (simplex.ContainsOrigin()) {
			return simplex;
		}

		direction = simplex.GetSearchDirection();
	}

	OSK_ASSERT(false, UnreachableException("XD"));
	return {};
}


void Simplex::AddAndUpdate(const MinkowskiSupport& point) {
	AddPoint(point);

	OSK_ASSERT(m_verticesCount <= 4, InvalidObjectStateException("Se ha excedido el número de vértices del simplex"));

	switch (m_verticesCount) {

	case 1:
		UpdatePoint();
		break;

	case 2:
		UpdateLine();
		break;

	case 3:
		UpdateTriangle();
		break;

	case 4:
		UpdateTetrahedron();
		break;

	default:
		break;
	}
}

void Simplex::UpdatePoint() {
	m_containsOrigin = false;

	const Vector3f& a = m_vertices[0].point.ToVector3f();
	const Vector3f ao = -a;

	m_direction = ao.GetNormalized();
}

void Simplex::UpdateLine() {
	m_containsOrigin = false;

	// Sólamente tenemos una línea:
	// 
	// - No puede estar en la dirección del vértice antiguo, ya que
	// para encontrar el origen de coordenadas debimos buscar en la
	// dirección contraria.
	// 
	// - No puede estar más allá del nuevo vértice, ya que en cuyo
	// caso no habría colisión.
	//
	// Por esto, simplemente buscamos un vector perpendicular a la
	// línea, en dirección al origen de coordenadas.

	const Vector3f& a = m_vertices[0].point.ToVector3f();
	const Vector3f& b = m_vertices[1].point.ToVector3f();

	const Vector3f ab = b - a;
	const Vector3f ao = -a;

	m_direction = ab.Cross(ao).Cross(ab).GetNormalized();
}

void Simplex::UpdateTriangle() {
	m_containsOrigin = false;

	const MinkowskiSupport& a = m_vertices[0];
	const MinkowskiSupport& b = m_vertices[1];
	const MinkowskiSupport& c = m_vertices[2];

	const Vector3f ab =  b.point.ToVector3f() - a.point.ToVector3f();
	const Vector3f ac =  c.point.ToVector3f() - a.point.ToVector3f();
	const Vector3f ao = -a.point.ToVector3f();

	const Vector3f abc = ab.Cross(ac);

//#define OSK_GJK_ULTRA_OPT
//#define OSK_GJKOPTIMIZED
#ifdef OSK_GJK_ULTRA_OPT
	// Caso encima triángulo:
	if (IsSameDirection(abc, ao)) {
		m_direction = abc.GetNormalized();
	}
	// Caso debajo triángulo:
	else {
		m_vertices = { a, c, b };
		m_direction = -abc.GetNormalized();
	}
#elif OSK_GJKOPTIMIZED
	// Caso línea AB:
	if (IsSameDirection(normal.Cross(ab), ao)) {
		m_points.Empty();
		m_points.Insert(a);
		m_points.Insert(b);

		m_direction = ab.Cross(ao).Cross(ab);
	}
	else
		// Caso línea AC:
		if (IsSameDirection(ac.Cross(normal), ao)) {
			m_points.Empty();
			m_points.Insert(a);
			m_points.Insert(c);

			m_direction = ac.Cross(ao).Cross(ac);
		}
		else
			// Caso encima triángulo:
			if (IsSameDirection(normal, ao)) {
				m_points.Empty();
				m_points.Insert(a);
				m_points.Insert(b);
				m_points.Insert(c);

				m_direction = normal;
			}
	// Caso debajo triángulo:
			else {
				m_points.Empty();
				m_points.Insert(a);
				m_points.Insert(c);
				m_points.Insert(b);

				m_direction = -normal;
			}
#else // OSK_GJKOPTIMIZED

	// Línea AC (hacia afuera):
	if (IsSameDirection(abc.Cross(ac), ao)) {

		// Entre A y C:
		if (IsSameDirection(ac, ao)) {
			m_vertices = { a, c };
			m_verticesCount = 2;

			m_direction = ac.Cross(ao).Cross(ac);
		}
		// C no apunta hacia el origen.
		else {
			m_vertices = { a, b };
			m_verticesCount = 2;
		}

	}
	// Línea AC (hacia dentro):
	else {

		// Línea AB (hacia afuera):
		if (IsSameDirection(ab.Cross(abc), ao)) {
			m_vertices = { a, b };
			m_verticesCount = 2;
		}
		else {
			// Encima del triángulo:
			if (IsSameDirection(abc, ao)) {
				m_direction = abc;
			}

			// Debajo del triángulo:
			else if (IsSameDirection(-abc, ao)) {
				m_vertices = { a, c, b };
				m_verticesCount = 3;

				m_direction = -abc;
			}
		}
	}

#endif
	
}

void Simplex::UpdateTetrahedron() {
	const MinkowskiSupport& a = m_vertices[0];
	const MinkowskiSupport& b = m_vertices[1];
	const MinkowskiSupport& c = m_vertices[2];
	const MinkowskiSupport& d = m_vertices[3];

	// TODO: comprobar si es necesario normalizar
	// ab, ac, ad.
	const Vector3f ab =  b.point.ToVector3f() - a.point.ToVector3f();
	const Vector3f ac =  c.point.ToVector3f() - a.point.ToVector3f();
	const Vector3f ad =  d.point.ToVector3f() - a.point.ToVector3f();
	const Vector3f ao = -a.point.ToVector3f().GetNormalized();

	const Vector3f abc = ab.Cross(ac).GetNormalized();
	const Vector3f acd = ac.Cross(ad).GetNormalized();
	const Vector3f adb = ad.Cross(ab).GetNormalized();


	// Triángulos
	if (IsSameDirection(abc, ao)) {
		m_vertices = { a, b, c };
		m_verticesCount = 3;
		m_direction = abc.GetNormalized();
	} else if (IsSameDirection(acd, ao)) {
		m_vertices = { a, c, d };
		m_verticesCount = 3;
		m_direction = acd.GetNormalized();
	} else if (IsSameDirection(adb, ao)) {
		m_vertices = { a, d, b };
		m_verticesCount = 3;
		m_direction = adb.GetNormalized();
	}
	else {
		m_containsOrigin = true;
	}
}

Vector3f Simplex::GetSearchDirection() const {
	return m_direction.GetNormalized();
}

std::span<const MinkowskiSupport> Simplex::GetVertices() const {
	return m_vertices;
}

USize64 Simplex::GetVerticesCount() const {
	return m_verticesCount;
}

bool Simplex::ContainsOrigin() const {
	// Cuando un vértice / línea / cara está en el origen de coordenadas,
	// es posible que la distancia sea (0, 0, 0).
	return m_containsOrigin;
}

void Simplex::AddPoint(const MinkowskiSupport& point) {
	m_vertices = {
		point,
		m_vertices[0],
		m_vertices[1],
		m_vertices[2]
	};

	m_verticesCount++;
}

bool Simplex::IsSameDirection(const Vector3f& a, const Vector3f& b) {
	return a.Dot(b) >= 0.0f;
}
