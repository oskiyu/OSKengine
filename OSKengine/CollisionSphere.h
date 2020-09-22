#pragma once

#include "Vector3.h"

namespace OSK {

	struct CollisionBox;

	//Representa un volumen esférico.
	//Puede usarse para detectar colisiones.
	struct CollisionSphere {

	public:

		//Crea una CollisionSphere vacía.
		CollisionSphere();

		//Crea una CollisionSphere.
		//	<position>: posición de la esfera en el mundo.
		//	<radius>: radio de la esfera.
		CollisionSphere(const Vector3f& position, const float_t& radius);

		//Comprueba si un punto está dentro de la esfera.
		//	<point>: punto 3D.
		bool ContainsPoint(const Vector3f& point) const;

		//Comprueba si esta esfera está en contacto con otra.
		//	<sphere>: otra esfera.
		bool Intersects(const CollisionSphere& sphere) const;

		//Comprueba si esta esfera está en contacto con una CollisionBox.
		//	<box>: CollisionBox.
		bool Intersects(const CollisionBox& box) const;

		//Posición de la esfera.
		Vector3f Position = { 0.0f };

		//Radio de la esfera.
		float_t Radius = 0.0f;

	};

}