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
	
	//C�mara para un mundo tridimensional.
	class OSKAPI_CALL Camera3D {

	public:

		//Crea una c�mara en la posici�n { posX, posY, posZ }.
		Camera3D(const cameraVar_t& posX, const cameraVar_t& posY, const cameraVar_t posZ);


		//Crea una c�mara.
		//<position>: posici�n de la c�mara.
		//<up>: vector 3D que determina los ejes de coordenadas de la c�mara.
		Camera3D(const Vector3& position, const Vector3& up);


		//Destruye la c�mara.
		~Camera3D();


		//Mueve la c�mara en una determinada direcci�n.
		void Move(const Directions& direccion, const deltaTime_t& deltaTime);


		//Gira la c�mara.
		//<pitch>: �ngulo en Y.
		//<yaw>: �ngulo en X.
		void Girar(const double& pitch, const double& yaw, const bool& constraint = true);


		//Establece el FoV de la c�mara, si est� en los l�mites de FoV.
		void SetFoV(const cameraVar_t& fov);


		//A�ade FoV a la c�mara.
		void AddFoV(const cameraVar_t& fov);


		glm::mat4 GetProjection() const;


		glm::mat4 GetView() const;

		
		//FoV de la c�mara.
		//~Zoom de la c�mara.
		cameraVar_t FieldOfView = static_cast<cameraVar_t>(45.0f);


		//L�mite del FoV de la c�mara.
		cameraVar_t FoVLimitDown = static_cast<cameraVar_t>(1.0f);


		//L�mite del FoV de la c�mara.
		cameraVar_t FoVLimitUp = static_cast<cameraVar_t>(45.0f);


		//Velocidad de movimiento de la c�mara.
		cameraVar_t Speed = static_cast<cameraVar_t>(2.5);


		//Sensibilidad al girar la c�mara.
		cameraVar_t Sensitivity = static_cast<cameraVar_t>(0.1);


		//Vector direcci�n de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Front = OSK::Vector3(static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(-1));


		//Vector 'arriba' de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Up = OSK::Vector3(static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(1), static_cast<cameraVar_t>(0));


		//Vector 'derecho' de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Right;


		//Vector 'arriba' del mundo. { 0, 1, 0 }
		const Vector3 WorldUp = OSK::Vector3(static_cast<cameraVar_t>(0), static_cast<cameraVar_t>(1), static_cast<cameraVar_t>(0));


		//Transform de la c�mara.
		Transform CameraTransform;


		WindowAPI* Window;
		
	private:

		void updateVectors();

	};

}