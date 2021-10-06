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
	/// Cámara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL Camera3D {

		friend class RenderAPI;
		friend class Game;

	public:

		/// <summary>
		/// Crea una cámara en la posición dada (coordenadas del mundo).
		/// </summary>
		/// <param name="posX">Posición X.</param>
		/// <param name="posY">Posición Y.</param>
		/// <param name="posZ">Posición Z.</param>
		Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ);

		/// <summary>
		/// Crea una cámara.
		/// </summary>
		/// <param name="position">Posición.</param>
		/// <param name="up">Dirección que apunta hacia arriba de la cámara.</param>
		Camera3D(const Vector3f& position, const Vector3f& up);

		/// <summary>
		/// Destruye la cámara.
		/// </summary>
		~Camera3D();

		/// <summary>
		/// Gira la cámara.
		/// </summary>
		/// <param name="pitch">Ángulo en Y.</param>
		/// <param name="yaw">Ángulo en X.</param>
		/// <param name="constraint">Si es true, no se puede estar boca abajo.</param>
		void Girar(double pitch, double yaw, bool constraint = true);

		/// <summary>
		/// Establece el FoV de la cámara, si está en los límites de FoV.
		/// </summary>
		/// <param name="fov">FoV.</param>
		void SetFov(double fov);

		/// <summary>
		/// Añade FoV a la cámara.
		/// </summary>
		/// <param name="fov">FoV delta.</param>
		void AddFov(double fov);

		/// <summary>
		/// Obtiene la matriz proyección de la cámara.
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetProjection() const;

		/// <summary>
		/// Obtiene la matriz view de la cámara.
		/// </summary>
		/// <returns></returns>
		glm::mat4 GetView() const;

		/// <summary>
		/// Límite del FoV de la cámara.
		/// </summary>
		float_t fovLimitDown = 1.0f;

		/// <summary>
		/// Límite del FoV de la cámara.
		/// </summary>
		float_t fovLimitUp = 45.0f;

		/// <summary>
		/// Transform de la cámara.
		/// </summary>
		Transform* GetTransform();

	private:

		/// <summary>
		/// FoV de la cámara.
		/// ~Zoom de la cámara.
		/// </summary>
		float_t fieldOfView = 45.0f;

		/// <summary>
		/// Vector 'arriba' del mundo. { 0, 1, 0 }.
		/// </summary>
		const Vector3f worldUp = Vector3f(0.0f, 1.0f, 0.0f);

		/// <summary>
		/// Transform de la cámara.
		/// </summary>
		Transform transform;

		/// <summary>
		/// Ventana renderizada.
		/// </summary>
		WindowAPI* window = nullptr;

	};

}