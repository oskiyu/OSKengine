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
		Camera3D(const Vector3f& position, const Vector3f& up);


		//Destruye la c�mara.
		~Camera3D();


		//Gira la c�mara.
		//<pitch>: �ngulo en Y.
		//<yaw>: �ngulo en X.
		void Girar(const double& pitch, const double& yaw, const bool& constraint = true);


		//Establece el FoV de la c�mara, si est� en los l�mites de FoV.
		void SetFoV(const double& fov);


		//A�ade FoV a la c�mara.
		void AddFoV(const double& fov);

		//Obtiene la matriz proyecci�n de la c�mara.
		glm::mat4 GetProjection() const;

		//Obtiene la matriz view de la c�mara.
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
			Vector3f  Front = Vector3f(0.0, 0.0, -1.0);


		//Vector 'arriba' de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3f Up = Vector3f(0.0, 1.0, 0.0);


		//Vector 'derecho' de la c�mara.
		OSK_INFO_DO_NOT_TOUCH
			Vector3f Right;


		//Vector 'arriba' del mundo. { 0, 1, 0 }
		const Vector3f WorldUp = Vector3f(0.0f, 1.0f, 0.0f);


		//Transform de la c�mara.
		Transform CameraTransform;


		WindowAPI* Window;

		float AngleX = 0.0f;

		float AngleY = 0.0f;

		void updateVectors();
	private:


	};

}