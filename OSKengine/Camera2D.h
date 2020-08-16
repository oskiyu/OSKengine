#pragma once

#include "WindowAPI.h"
#include "Vector2.h"
#include "Transform2D.h"
#include <glm.hpp>

namespace OSK {

	//Cámara para un mundo de dos dimensiones.
	class OSKAPI_CALL Camera2D {
		friend class VulkanRenderer;

	public:

		//Crea la cámara.
		Camera2D();

		//Crea una cámara, para esta ventana.
		Camera2D(WindowAPI* window);

		//Actualiza los valores de la cámara.
		void Update();

		//Transform de la cámara.
		Transform2D CameraTransform;

		//TRUE: cambiar el tamaño de la ventana no cambia el área de la cámara.
		//FALSE: cambiar el tamaño de la ventana sí cambia el área de la cámara.
		bool UseTargetSize = false;

		//Transforma un punto 2D en la ventana al punto 2D al que apunta la cámara en el mundo 2D.
		Vector2 PointInWindowToPointInWorld(const Vector2& point) const;

		//Área de la cámara.
		Vector2 TargetSize{};

	private:

		WindowAPI* window = nullptr;

		glm::mat4 projection;

	};

}