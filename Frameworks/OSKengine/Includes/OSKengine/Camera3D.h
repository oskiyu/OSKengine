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
	/// C�mara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL Camera3D {

	public:

		//Crea una c�mara en la posici�n { posX, posY, posZ }.

		/// <summary>
		/// Crea una c�mara en la posici�n dada (coordenadas del mundo).
		/// </summary>
		/// <param name="posX">Posici�n X.</param>
		/// <param name="posY">Posici�n Y.</param>
		/// <param name="posZ">Posici�n Z.</param>
		Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ);

		//Crea una c�mara.
		//<position>: posici�n de la c�mara.
		//<up>: vector 3D que determina los ejes de coordenadas de la c�mara.

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

		//Gira la c�mara.
		//<pitch>: �ngulo en Y.
		//<yaw>: �ngulo en X.

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
		void SetFoV(double fov);

		/// <summary>
		/// A�ade FoV a la c�mara.
		/// </summary>
		/// <param name="fov">FoV delta.</param>
		void AddFoV(double fov);

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
		/// FoV de la c�mara.
		/// ~Zoom de la c�mara.
		/// </summary>
		float_t FieldOfView = 45.0f;

		/// <summary>
		/// L�mite del FoV de la c�mara.
		/// </summary>
		float_t FoVLimitDown = 1.0f;

		/// <summary>
		/// L�mite del FoV de la c�mara.
		/// </summary>
		float_t FoVLimitUp = 45.0f;

		/// <summary>
		/// Vector direcci�n de la c�mara.
		/// </summary>
		Vector3f  Front = Vector3f(0.0, 0.0, -1.0);

		/// <summary>
		/// Vector 'arriba' de la c�mara.
		/// </summary>
		Vector3f Up = Vector3f(0.0, 1.0, 0.0);

		/// <summary>
		/// Vector 'derecho' de la c�mara.
		/// </summary>
		Vector3f Right;

		/// <summary>
		/// Vector 'arriba' del mundo. { 0, 1, 0 }.
		/// </summary>
		const Vector3f WorldUp = Vector3f(0.0f, 1.0f, 0.0f);

		/// <summary>
		/// Transform de la c�mara.
		/// </summary>
		Transform CameraTransform;

		/// <summary>
		/// Ventana renderizada.
		/// </summary>
		WindowAPI* Window = nullptr;

		/// <summary>
		/// �ngulo en X.
		/// </summary>
		float AngleX = 0.0f;

		/// <summary>
		/// �ngulo en Y.
		/// </summary>
		float AngleY = 0.0f;

		/// <summary>
		/// Actualiza los vectores internos.
		/// </summary>
		void updateVectors();

	};

}