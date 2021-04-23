#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"
#include "WindowAPI.h"
#include "UBO.h"

namespace OSK {

	/// <summary>
	/// Cámara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL Camera3D {

	public:

		//Crea una cámara en la posición { posX, posY, posZ }.

		/// <summary>
		/// Crea una cámara en la posición dada (coordenadas del mundo).
		/// </summary>
		/// <param name="posX">Posición X.</param>
		/// <param name="posY">Posición Y.</param>
		/// <param name="posZ">Posición Z.</param>
		Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ);

		//Crea una cámara.
		//<position>: posición de la cámara.
		//<up>: vector 3D que determina los ejes de coordenadas de la cámara.

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

		//Gira la cámara.
		//<pitch>: ángulo en Y.
		//<yaw>: ángulo en X.

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
		void SetFoV(double fov);

		/// <summary>
		/// Añade FoV a la cámara.
		/// </summary>
		/// <param name="fov">FoV delta.</param>
		void AddFoV(double fov);

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
		/// FoV de la cámara.
		/// ~Zoom de la cámara.
		/// </summary>
		float_t FieldOfView = 45.0f;

		/// <summary>
		/// Límite del FoV de la cámara.
		/// </summary>
		float_t FoVLimitDown = 1.0f;

		/// <summary>
		/// Límite del FoV de la cámara.
		/// </summary>
		float_t FoVLimitUp = 45.0f;

		/// <summary>
		/// Vector dirección de la cámara.
		/// </summary>
		Vector3f  Front = Vector3f(0.0, 0.0, -1.0);

		/// <summary>
		/// Vector 'arriba' de la cámara.
		/// </summary>
		Vector3f Up = Vector3f(0.0, 1.0, 0.0);

		/// <summary>
		/// Vector 'derecho' de la cámara.
		/// </summary>
		Vector3f Right;

		/// <summary>
		/// Vector 'arriba' del mundo. { 0, 1, 0 }.
		/// </summary>
		const Vector3f WorldUp = Vector3f(0.0f, 1.0f, 0.0f);

		/// <summary>
		/// Transform de la cámara.
		/// </summary>
		Transform CameraTransform;

		/// <summary>
		/// Ventana renderizada.
		/// </summary>
		WindowAPI* Window = nullptr;

		/// <summary>
		/// Ángulo en X.
		/// </summary>
		float AngleX = 0.0f;

		/// <summary>
		/// Ángulo en Y.
		/// </summary>
		float AngleY = 0.0f;

		/// <summary>
		/// Actualiza los vectores internos.
		/// </summary>
		void updateVectors();

	};

}