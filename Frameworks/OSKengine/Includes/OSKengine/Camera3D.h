#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"
#include "WindowAPI.h"
#include "UBO.h"

class Game;

namespace OSK {

	/// <summary>
	/// C�mara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL Camera3D {

		friend class RenderAPI;
		friend class Game;

	public:

		/// <summary>
		/// Crea una c�mara en la posici�n dada (coordenadas del mundo).
		/// </summary>
		/// <param name="posX">Posici�n X.</param>
		/// <param name="posY">Posici�n Y.</param>
		/// <param name="posZ">Posici�n Z.</param>
		Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ);

		/// <summary>
		/// Crea una c�mara.
		/// </summary>
		/// <param name="position">Posici�n.</param>
		/// <param name="up">Direcci�n que apunta hacia arriba de la c�mara.</param>
		Camera3D(const Vector3f& position, const Vector3f& up);

		/// <summary>
		/// Destruye la c�mara.
		/// </summary>
		~Camera3D();

		/// <summary>
		/// Gira la c�mara.
		/// </summary>
		/// <param name="pitch">�ngulo en Y.</param>
		/// <param name="yaw">�ngulo en X.</param>
		/// <param name="constraint">Si es true, no se puede estar boca abajo.</param>
		void Girar(double pitch, double yaw, bool constraint = true);

		/// <summary>
		/// Establece el FoV de la c�mara, si est� en los l�mites de FoV.
		/// </summary>
		/// <param name="fov">FoV.</param>
		void SetFov(double fov);

		/// <summary>
		/// A�ade FoV a la c�mara.
		/// </summary>
		/// <param name="fov">FoV delta.</param>
		void AddFov(double fov);

		/// <summary>
		/// Obtiene la matriz proyecci�n de la c�mara.
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection() const;

		/// <summary>
		/// Obtiene la matriz view de la c�mara.
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView() const;

		/// <summary>
		/// L�mite del FoV de la c�mara.
		/// </summary>
		float_t fovLimitDown = 1.0f;

		/// <summary>
		/// L�mite del FoV de la c�mara.
		/// </summary>
		float_t fovLimitUp = 45.0f;

		/// <summary>
		/// Transform de la c�mara.
		/// </summary>
		Transform* GetTransform();

	private:

		/// <summary>
		/// FoV de la c�mara.
		/// ~Zoom de la c�mara.
		/// </summary>
		float_t fieldOfView = 45.0f;

		/// <summary>
		/// Vector 'arriba' del mundo. { 0, 1, 0 }.
		/// </summary>
		const Vector3f worldUp = Vector3f(0.0f, 1.0f, 0.0f);

		/// <summary>
		/// Transform de la c�mara.
		/// </summary>
		Transform transform;

		/// <summary>
		/// Ventana renderizada.
		/// </summary>
		WindowAPI* window = nullptr;

	};

}