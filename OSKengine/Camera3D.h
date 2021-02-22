#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"
#include "WindowAPI.h"
#include "UBO.h"

namespace OSK {


	//Cámara para un mundo tridimensional.
	class OSKAPI_CALL Camera3D {

	public:

		//Crea una cámara en la posición { posX, posY, posZ }.
		Camera3D(cameraVar_t posX, cameraVar_t posY, cameraVar_t posZ);

		//Crea una cámara.
		//<position>: posición de la cámara.
		//<up>: vector 3D que determina los ejes de coordenadas de la cámara.
		Camera3D(const Vector3f& position, const Vector3f& up);

		//Destruye la cámara.
		~Camera3D();

		//Gira la cámara.
		//<pitch>: ángulo en Y.
		//<yaw>: ángulo en X.
		void Girar(double pitch, double yaw, bool constraint = true);

		//Establece el FoV de la cámara, si está en los límites de FoV.
		void SetFoV(double fov);

		//Añade FoV a la cámara.
		void AddFoV(double fov);

		//Obtiene la matriz proyección de la cámara.
		glm::mat4 GetProjection() const;
		//Obtiene la matriz view de la cámara.
		glm::mat4 GetView() const;

		//FoV de la cámara.
		//~Zoom de la cámara.
		float_t FieldOfView = 45.0f;

		//Límite del FoV de la cámara.
		float_t FoVLimitDown = 1.0f;
		//Límite del FoV de la cámara.
		float_t FoVLimitUp = 45.0f;

		//Vector dirección de la cámara.
		Vector3f  Front = Vector3f(0.0, 0.0, -1.0);
		//Vector 'arriba' de la cámara.
		Vector3f Up = Vector3f(0.0, 1.0, 0.0);
		//Vector 'derecho' de la cámara.
		Vector3f Right;

		//Vector 'arriba' del mundo. { 0, 1, 0 }
		const Vector3f WorldUp = Vector3f(0.0f, 1.0f, 0.0f);

		//Transform de la cámara.
		Transform CameraTransform;

		WindowAPI* Window;

		float AngleX = 0.0f;
		float AngleY = 0.0f;

		void updateVectors();

	private:


	};

}