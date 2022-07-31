#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Component.h"

namespace OSK::ECS {

	class Transform3D;

	/// <summary>
	/// Cámara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL CameraComponent3D {

	public:

		OSK_COMPONENT("OSK::CameraComponent3D");

		/// <summary>
		/// FoV de la cámara.
		/// 
		/// @note Zoom de la cámara.
		/// </summary>
		float GetFov() const;

		/// <summary>
		/// Establece el FoV de la cámara, si está en los límites de FoV.
		/// 
		/// @note Si el valor está fuera de los límites, se pondrá el valor válido
		/// más cercano.
		/// </summary>
		void SetFov(float fov);

		/// <summary>
		/// Añade FoV a la cámara.
		/// </summary>
		/// <param name="diff">FoV delta.</param>
		void AddFov(float diff);

		/// <summary>
		/// Establece los límites mínimos y máximos de field of view.
		/// </summary>
		/// 
		/// @throws std::runtime_exception Si min >= max.
		void SetFovLimits(float min, float max);


		/// <summary>
		/// Obtiene el límite inferior de distancia a partir de la cual
		/// se renderizan objetos.
		/// </summary>
		float GetNearPlane() const;

		/// <summary>
		/// Obtiene el límite superior de distancia a partir de la cual
		/// no se renderizan objetos.
		/// </summary>
		float GetFarPlane() const;

		/// <summary>
		/// Establece los límites de distancia de renderizado.
		/// Se renderizarán los objetos que estén dentro de los
		/// límites.
		/// </summary>
		/// 
		/// @throws std::runtime_exception si near >= far.
		void SetPlanes(float near, float far);


		/// <summary>
		/// Rota la cámara.
		/// </summary>
		/// <param name="angleX">Yaw.</param>
		/// <param name="angleY">Pitch.</param>
		void Rotate(float angleX, float angleY);
		
		/// <summary>
		/// Actualiza el transform de la cámara, para que tenga
		/// su rotación.
		/// </summary>
		void UpdateTransform(Transform3D* transform);


		/// <summary>
		/// Obtiene la matriz proyección de la cámara.
		/// </summary>
		glm::mat4 GetProjectionMatrix(const Transform3D& transform) const;

		/// <summary>
		/// Obtiene la matriz view de la cámara.
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
