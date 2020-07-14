#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DirectionsEnum.h"
#include "Shader.h"
#include "Transform.h"
#include "WindowAPI.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>

namespace OSK {
	
	//Cámara para un mundo tridimensional.
	class OSKAPI_CALL Camera3D {

	public:

		//Crea una cámara en la posición { posX, posY, posZ }.
		Camera3D(const cameraVar_t& posX, const cameraVar_t& posY, const cameraVar_t posZ);


		//Crea una cámara.
		//<position>: posición de la cámara.
		//<up>: vector 3D que determina los ejes de coordenadas de la cámara.
		Camera3D(const Vector3& position, const Vector3& up);


		//Destruye la cámara.
		~Camera3D();


		//Mueve la cámara en una determinada dirección.
		void Move(const Directions& direccion, const deltaTime_t& deltaTime);


		//Gira la cámara.
		//<pitch>: ángulo en Y.
		//<yaw>: ángulo en X.
		void Girar(const double& pitch, const double& yaw, const bool& constraint = true);


		//Establece el FoV de la cámara, si está en los límites de FoV.
		void SetFoV(const cameraVar_t& fov);


		//Añade FoV a la cámara.
		void AddFoV(const cameraVar_t& fov);


		glm::mat4 GetProjection() const;


		glm::mat4 GetView() const;

		
		//FoV de la cámara.
		//~Zoom de la cámara.
		cameraVar_t FieldOfView = static_cast<cameraVar_t>(45.0f);


		//Límite del FoV de la cámara.
		cameraVar_t FoVLimitDown = static_cast<cameraVar_t>(1.0f);


		//Límite del FoV de la cámara.
		cameraVar_t FoVLimitUp = static_cast<cameraVar_t>(45.0f);


		//Velocidad de movimiento de la cámara.
		cameraVar_t Speed = static_cast<cameraVar_t>(2.5);


		//Sensibilidad al girar la cámara.
		cameraVar_t Sensitivity = static_cast<cameraVar_t>(0.1);


		//Vector dirección de la cámara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Front = OSK::Vector3(static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(-1));


		//Vector 'arriba' de la cámara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Up = OSK::Vector3(static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(1), static_cast<cameraVar_t>(0));


		//Vector 'derecho' de la cámara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Right;


		//Vector 'arriba' del mundo. { 0, 1, 0 }
		const Vector3 WorldUp = OSK::Vector3(static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(1), static_cast<cameraVar_t>(0));


		//Transform de la cámara.
		Transform CameraTransform;


		WindowAPI* Window;
		
	private:

		void updateVectors();

	};

}