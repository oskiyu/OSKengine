#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	struct CollisionSphere;

	/// <summary>
	/// Representa un volumen 3D c�bico.
	/// Puede usarse para detectar colisiones.
	/// </summary>
	struct OSKAPI_CALL CollisionBox {

	public:

		/// <summary>
		/// Crea un colision box.
		/// </summary>
		CollisionBox();

		/// <summary>
		/// Crea una CollisionBox.
		/// </summary>
		/// <param name="position">Posici�n de la caja en el mundo.</param>
		/// <param name="size">Tama�o de la caja.</param>
		CollisionBox(const Vector3f& position, const Vector3f& size);

		/// <summary>
		/// Comprueba si un punto est� dentro de la caja.
		/// </summary>
		/// <param name="point">Punto 3D.</param>
		/// <returns>True si el punto est� dentro de la caja.</returns>
		bool ContainsPoint(const Vector3f& point) const;

		/// <summary>
		/// Comprueba si esta caja est� en contacto con otra.
		/// </summary>
		/// <param name="box">Otra caja.</param>
		/// <returns>True si colisionan.</returns>
		bool Intersects(const CollisionBox& box) const;

		/// <summary>
		/// Comprueba si esta caja est� en contacto con una CollisionSphere.
		/// </summary>
		/// <param name="sphere">CollisionSphere.</param>
		/// <returns>True si colisionan.</returns>
		bool Intersects(const CollisionSphere& sphere) const;

		/// <summary>
		/// Obtiene la posici�n del origen.
		/// </summary>
		/// <returns>Position - size.</returns>
		Vector3f GetMin() const;

		/// <summary>
		/// Obtiene la posici�n de la esquina contraria al origen.
		/// Position + Size.
		/// </summary>
		/// <returns>Position + Size.</returns>
		Vector3f GetMax() const;

		/// <summary>
		/// Posici�n de la caja.
		/// </summary>
		Vector3f Position;

		/// <summary>
		/// Tama�o de la caja.
		/// </summary>
		Vector3f Size = { 1.0f };

	};

}
