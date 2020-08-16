#pragma once

#include "WindowAPI.h"
#include "Vector2.h"
#include "Transform2D.h"
#include <glm.hpp>

namespace OSK {

	//C�mara para un mundo de dos dimensiones.
	class OSKAPI_CALL Camera2D {
		friend class VulkanRenderer;

	public:

		//Crea la c�mara.
		Camera2D();

		//Crea una c�mara, para esta ventana.
		Camera2D(WindowAPI* window);

		//Actualiza los valores de la c�mara.
		void Update();

		//Transform de la c�mara.
		Transform2D CameraTransform;

		//TRUE: cambiar el tama�o de la ventana no cambia el �rea de la c�mara.
		//FALSE: cambiar el tama�o de la ventana s� cambia el �rea de la c�mara.
		bool UseTargetSize = false;

		//Transforma un punto 2D en la ventana al punto 2D al que apunta la c�mara en el mundo 2D.
		Vector2 PointInWindowToPointInWorld(const Vector2& point) const;

		//�rea de la c�mara.
		Vector2 TargetSize{};

	private:

		WindowAPI* window = nullptr;

		glm::mat4 projection;

	};

}