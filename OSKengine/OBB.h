#pragma once

#include "OSKtypes.h"

#include "Transform.h"

#include <array>

namespace OSK {

	namespace Collision {

		//Representa la proyección de un OBB sobre un eje.
		struct OBB_Projection3D {
			float min;
			float max;
		};

		//Devuelve true si una proyección toca a otra.
		//Solo debe usarse con proyecciones sobre un mismo eje.
		//	<a>: proyección 1.
		//	<b>: proyección 2.
		inline bool porjectionOverlaps(const OBB_Projection3D& a, const OBB_Projection3D& b) {
			return a.max > b.min && a.min < b.max;
		}

	}

	//Oriented Bounding Box.
	//Para detección de colisiones con cajas que pueden rotar.
	class OBB {
	
	public:
		//Devuelve true si toca a otra OBB.
		//	<obb>: el otro OBB.
		bool Intersects(const OBB& obb) const;

		//Transform de este OBB.
		Transform BoxTransform{};

	private:

		//Proyecta este OBB sobre un eje.
		//	<axis>: eje sobre el que se proyecta.
		Collision::OBB_Projection3D ProjectToAxis(const Vector3f& axis) const;

		//Devuelve los ejes de este OBB.
		std::array<Vector3f, 3> GetAxes() const;

		//Devuelve la posición de los puntos de est OBB.
		std::array<Vector3f, 8> GetPoints() const;

	};

}
