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
		Camera3D(const cameraVar_t& posX, const cameraVar_t& posY, const cameraVar_t posZ);


		//Crea una cámara.
		//<position>: posición de la cámara.
		//<up>: vector 3D que determina los ejes de coordenadas de la cámara.
		Camera3D(const Vector3_t<double>& position, const Vector3_t<double>& up);


		//Destruye la cámara.
		~Camera3D();


		//Gira la cámara.
		//<pitch>: ángulo en Y.
		//<yaw>: ángulo en X.
		void Girar(const double& pitch, const double& yaw, const bool& constraint = true);


		//Establece el FoV de la cámara, si está en los límites de FoV.
		void SetFoV(const double& fov);


		//Añade FoV a la cámara.
		void AddFoV(const double& fov);


		glm::mat4 GetProjection() const;


		glm::mat4 GetView() const;


		//FoV de la cámara.
		//~Zoom de la cámara.
		float_t FieldOfView = 45.0f;


		//Límite del FoV de la cámara.
		float_t FoVLimitDown = 1.0f;


		//Límite del FoV de la cámara.
		float_t FoVLimitUp = 45.0f;


		//Vector dirección de la cámara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3_t<double> Front = OSK::Vector3_t<double>(0.0, 0.0, -1.0);


		//Vector 'arriba' de la cámara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3_t<double> Up = OSK::Vector3_t<double>(0.0, 1.0, 0.0);


		//Vector 'derecho' de la cámara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3_t<double> Right;


		//Vector 'arriba' del mundo. { 0, 1, 0 }
		const Vector3_t<double> WorldUp = OSK::Vector3_t<double>(0.0f, 1.0f, 0.0f);


		//Transform de la cámara.
		Transform_t<double> CameraTransform;


		WindowAPI* Window;

		void updateVectors();
	private:


	};

}