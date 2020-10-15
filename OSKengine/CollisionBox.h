#pragma once

#include "Vector3.hpp"

namespace OSK {

	struct CollisionSphere;

	//Representa un volumen 3D.
	//Puede usarse para detectar colisiones.
	struct CollisionBox {

	public:

		//Crea una CollisionBox vacía.
		CollisionBox();

		//Crea una CollisionBox.
		//	<position>: posición de la caja en el mundo.
		//	<size>: tamaño de la caja.
		CollisionBox(const Vector3f& position, const Vector3f& size);

		//Comprueba si un punto está dentro de la caja.
		//	<point>: punto 3D.
		bool ContainsPoint(const Vector3f& point) const;

		//Comprueba si esta caja está en contacto con otra.
		//	<box>: otra caja.
		bool Intersects(const CollisionBox& box) const;


		//Comprueba si esta caja está en contacto con una CollisionSphere.
		//	<sphere>: CollisionSphere.
		bool Intersects(const CollisionSphere& sphere) const;

		//Obtiene la posición de la esquina contraria al origen.
		//Position + Size.
		Vector3f GetMax() const;

		//Obtiene la posición de la esquina contraria al origen.
		//Position - Size.
		Vector3f GetMin() const;

		//Posición de la caja.
		Vector3f Position;

		//Tamaño de la caja.
		Vector3f Size;

	};

}
