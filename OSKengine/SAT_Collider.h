#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SAT_Projection.hpp"
#include "SAT_CollisionInfo.h"
#include "Transform.h"

#include <vector>

namespace OSK::Collision {

	//Lista de puntos que representan una cara.
	typedef std::vector<VectorElementPtr_t> SAT_Face;

	//Collider que usa el Teorema de Separación de Ejes.
	//Puede tener n caras, cada una con m puntos.
	class OSKAPI_CALL SAT_Collider {

	public:

		//Optimiza las caras, dejando sólamente las que tienen ejes únicos.
		void OptimizeFaces();

		//Añade una cara.
		//	<points>: array de puntos.
		//	<size>: número de puntos.
		void AddFace(const Vector3f points[], const uint32_t& size);

		//Comprueba si este collider choca con otro.
		//Más barato de ejecutar que GetCollisionInfo.
		//	<other>: otro sat collider.
		bool Intersects(const SAT_Collider& other) const;

		//Obtiene un SAT_CollisionInfo con la información de la intersección de este collider con otro.
		//	<other>: otro sat collider.
		SAT_CollisionInfo GetCollisionInfo(const SAT_Collider& other) const;

		//Actualiza la posición de los puntos en el mundo.
		void TransformPoints();

		//Transform de este OBB.
		Transform BoxTransform{};

		//Crea un SAT_Collider cúbico.
		static SAT_Collider CreateOBB();

		//Obtiene los puntos transformados en el mundo.
		std::vector<Vector3f> GetPoints() const;

		//Obtiene los ejes del collider.
		//Transformados teniendo en cuenta su posición y rotación.
		std::vector<Vector3f> GetAxes() const;

		//True si el collider no va a moverse, rotar o cambiar de tamaño.
		//Para la optimización de transformación de puntos.
		bool IsStatic = false;

		SAT_Projection ProjectToAxis(const Vector3f& axis) const;

	private:
		
		Vector3f GetAxisFromFace(const SAT_Face& face) const;

		std::vector<Vector3f> Points = {};
		std::vector<Vector3f> TransformedPoints = {};

		std::vector<SAT_Face> Faces;

	};

}