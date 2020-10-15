#pragma once

#include "Vector3.hpp"

namespace OSK {

	struct CollisionSphere;

	//Representa un volumen 3D.
	//Puede usarse para detectar colisiones.
	struct CollisionBox {

	public:

		//Crea una CollisionBox vac�a.
		CollisionBox();

		//Crea una CollisionBox.
		//	<position>: posici�n de la caja en el mundo.
		//	<size>: tama�o de la caja.
		CollisionBox(const Vector3f& position, const Vector3f& size);

		//Comprueba si un punto est� dentro de la caja.
		//	<point>: punto 3D.
		bool ContainsPoint(const Vector3f& point) const;

		//Comprueba si esta caja est� en contacto con otra.
		//	<box>: otra caja.
		bool Intersects(const CollisionBox& box) const;


		//Comprueba si esta caja est� en contacto con una CollisionSphere.
		//	<sphere>: CollisionSphere.
		bool Intersects(const CollisionSphere& sphere) const;

		//Obtiene la posici�n de la esquina contraria al origen.
		//Position + Size.
		Vector3f GetMax() const;

		//Obtiene la posici�n de la esquina contraria al origen.
		//Position - Size.
		Vector3f GetMin() const;

		//Posici�n de la caja.
		Vector3f Position;

		//Tama�o de la caja.
		Vector3f Size;

	};

}
