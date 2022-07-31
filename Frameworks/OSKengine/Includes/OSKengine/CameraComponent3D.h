#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Component.h"

namespace OSK::ECS {

	class Transform3D;

	/// <summary>
	/// C�mara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL CameraComponent3D {

	public:

		OSK_COMPONENT("OSK::CameraComponent3D");

		/// <summary>
		/// FoV de la c�mara.
		/// 
		/// @note Zoom de la c�mara.
		/// </summary>
		float GetFov() const;

		/// <summary>
		/// Establece el FoV de la c�mara, si est� en los l�mites de FoV.
		/// 
		/// @note Si el valor est� fuera de los l�mites, se pondr� el valor v�lido
		/// m�s cercano.
		/// </summary>
		void SetFov(float fov);

		/// <summary>
		/// A�ade FoV a la c�mara.
		/// </summary>
		/// <param name="diff">FoV delta.</param>
		void AddFov(float diff);

		/// <summary>
		/// Establece los l�mites m�nimos y m�ximos de field of view.
		/// </summary>
		/// 
		/// @throws std::runtime_exception Si min >= max.
		void SetFovLimits(float min, float max);


		/// <summary>
		/// Obtiene el l�mite inferior de distancia a partir de la cual
		/// se renderizan objetos.
		/// </summary>
		float GetNearPlane() const;

		/// <summary>
		/// Obtiene el l�mite superior de distancia a partir de la cual
		/// no se renderizan objetos.
		/// </summary>
		float GetFarPlane() const;

		/// <summary>
		/// Establece los l�mites de distancia de renderizado.
		/// Se renderizar�n los objetos que est�n dentro de los
		/// l�mites.
		/// </summary>
		/// 
		/// @throws std::runtime_exception si near >= far.
		void SetPlanes(float near, float far);


		/// <summary>
		/// Rota la c�mara.
		/// </summary>
		/// <param name="angleX">Yaw.</param>
		/// <param name="angleY">Pitch.</param>
		void Rotate(float angleX, float angleY);
		
		/// <summary>
		/// Actualiza el transform de la c�mara, para que tenga
		/// su rotaci�n.
		/// </summary>
		void UpdateTransform(Transform3D* transform);


		/// <summary>
		/// Obtiene la matriz proyecci�n de la c�mara.
		/// </summary>
		glm::mat4 GetProjectionMatrix(const Transform3D& transform) const;

		/// <summary>
		/// Obtiene la matriz view de la c�mara.
		/// </summary>
		glm::mat4 GetViewMatrix() const;

	private:

		float fov = 45.0f;
		float fovLimitDown = 1.0f;
		float fovLimitUp = 45.0f;

		float nearPlane = 0.001f;
		float farPlane = 5000.0f;

		Vector2f angles = 0.0f;
		Vector2f accumulatedAngles = 0.0f;

		/// <summary>
		/// Vector 'arriba' del mundo. { 0, 1, 0 }.
		/// </summary>
		const static Vector3f worldUpVector;

	};

}
