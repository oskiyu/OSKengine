#pragma once

#include "WindowAPI.h"
#include "Vector2.hpp"
#include "Transform2D.h"
#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// C�mara para un mundo de dos dimensiones.
	/// </summary>
	class OSKAPI_CALL Camera2D {

		friend class RenderAPI;
		friend class SpriteBatch;

	public:

		/// <summary>
		/// Crea la c�mara por defecto.
		/// </summary>
		Camera2D();

		/// <summary>
		/// Crea una c�mara, para esta ventana.
		/// </summary>
		/// <param name="window">Ventana renderizada.</param>
		Camera2D(WindowAPI* window);

		/// <summary>
		/// Actualiza los valores de la c�mara.
		/// </summary>
		void Update();

		/// <summary>
		/// Transform de la c�mara.
		/// </summary>
		Transform2D CameraTransform;

		/// <summary>
		/// TRUE: cambiar el tama�o de la ventana no cambia el �rea de la c�mara.
		/// FALSE: cambiar el tama�o de la ventana s� cambia el �rea de la c�mara.
		/// </summary>
		bool UseTargetSize = false;

		/// <summary>
		/// Transforma un punto 2D en la ventana al punto 2D al que apunta la c�mara en el mundo 2D.
		/// </summary>
		/// <param name="point">Punto en coordenadas de la ventana.</param>
		/// <returns>Punto, en coordenadas del mundo 2D.</returns>
		Vector2 PointInWindowToPointInWorld(const Vector2& point) const;

		/// <summary>
		/// �rea de la c�mara.
		/// </summary>
		Vector2 TargetSize{};

	private:

		/// <summary>
		/// Ventana.
		/// </summary>
		WindowAPI* window = nullptr;

		/// <summary>
		/// Matriz de la c�mara.
		/// </summary>
		glm::mat4 projection;

	};

}