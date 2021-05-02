#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	struct CollisionBox;

	/// <summary>
	/// Representa un volumen esf�rico.
	/// Puede usarse para detectar colisiones.
	/// </summary>
	struct OSKAPI_CALL CollisionSphere {

	public:

		/// <summary>
		/// Crea una CollisionSphere vac�a.
		/// </summary>
		CollisionSphere();

		/// <summary>
		/// Crea una CollisionSphere.
		/// </summary>
		/// <param name="position">Posici�n de la esfera en el mundo.</param>
		/// <param name="radius">Radio de la esfera.</param>
		CollisionSphere(const Vector3f& position, float radius);

		/// <summary>
		/// Comprueba si un punto est� dentro de la esfera.
		/// </summary>
		/// <param name="point">Punto 3D.</param>
		/// <returns>True si el punto est� dentro.</returns>
		bool ContainsPoint(const Vector3f& point) const;

		/// <summary>
		/// Comprueba si esta esfera est� en contacto con otra.
		/// </summary>
		/// <param name="sphere">Otra esfera.</param>
		/// <returns>True si colisionan.</returns>
		bool Intersects(const CollisionSphere& sphere) const;

		/// <summary>
		/// Comprueba si esta esfera est� en contacto con una CollisionBox.
		/// </summary>
		/// <param name="box">CollisionBox.</param>
		/// <returns>True si colisionan.</returns>
		bool Intersects(const CollisionBox& box) const;

		/// <summary>
		/// Posici�n de la esfera.
		/// </summary>
		Vector3f Position = { 0.0f };

		/// <summary>
		/// Radio de la esfera.
		/// </summary>
		float_t Radius = 0.0f;

	};

}