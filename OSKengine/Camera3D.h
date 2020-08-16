#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"
#include "WindowAPI.h"
#include "UBO.h"

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


		//Gira la c�mara.
		//<pitch>: �ngulo en Y.
		//<yaw>: �ngulo en X.
		void Girar(const float_t& pitch, const float_t& yaw, const bool& constraint = true);


		//Establece el FoV de la c�mara, si est� en los l�mites de FoV.
		void SetFoV(const float_t& fov);


		//A�ade FoV a la c�mara.
		void AddFoV(const float_t& fov);


		glm::mat4 GetProjection() const;


		glm::mat4 GetView() const;


		//FoV de la c�mara.
		//~Zoom de la c�mara.
		float_t FieldOfView = 45.0f;


		//L�mite del FoV de la c�mara.
		float_t FoVLimitDown = 1.0f;


		//L�mite del FoV de la c�mara.
		float_t FoVLimitUp = 45.0f;


		//Vector direcci�n de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Front = OSK::Vector3(0.0f, 0.0f, -1.0f);


		//Vector 'arriba' de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Up = OSK::Vector3(0.0f, 1.0f, 0.0f);


		//Vector 'derecho' de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3 Right;


		//Vector 'arriba' del mundo. { 0, 1, 0 }
		const Vector3 WorldUp = OSK::Vector3(0.0f, 1.0f, 0.0f);


		//Transform de la c�mara.
		Transform CameraTransform;


		WindowAPI* Window;

		void updateVectors();
	private:


	};

}