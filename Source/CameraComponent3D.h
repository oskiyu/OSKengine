#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Component.h"

#include "Frustum.h"

#include "Serializer.h"


namespace OSK::ECS {

	class Transform3D;

	/// <summary>
	/// Cámara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL CameraComponent3D {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

		OSK_COMPONENT("OSK::CameraComponent3D");

		/// <summary> FoV de la cámara. </summary>
		/// 
		/// @note Zoom de la cámara.
		float GetFov() const;

		/// <summary> Establece el FoV de la cámara, si está en los límites de FoV. </summary>
		/// 
		/// @note Si el valor está fuera de los límites, se pondrá el valor válido
		/// más cercano.
		void SetFov(float fov);

		/// <summary> Añade FoV a la cámara. </summary>
		/// <param name="diff">FoV delta.</param>
		/// 
		/// @note Si el valor está fuera de los límites, se pondrá el valor válido
		void AddFov(float diff);

		/// <summary>  Establece los límites mínimos y máximos de field of view. </summary>
		/// 
		/// @pre Max debe ser > min.
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
		/// @pre Max debe ser > min.
		/// @throws std::runtime_exception si near >= far.
		void SetPlanes(float near, float far);


		/// <summary> Rota la cámara. </summary>
		/// 
		/// <param name="angleX">Yaw.</param>
		/// <param name="angleY">Pitch.</param>
		void Rotate(float angleX, float angleY);
		
		/// <summary>
		/// Actualiza el transform de la cámara, para que tenga
		/// su rotación.
		/// </summary>
		/// 
		/// @pre transform no debe ser null.
		void UpdateTransform(Transform3D* transform);


		/// @param transform Transform de la cámara.
		/// @return Frustum de la cámara.
		AnyFrustum GetFrustum(const Transform3D& transform) const;


		/// @return Matriz proyección de la cámara, con depth Z invertido y
		/// far-plane infinito.
		glm::mat4 GetProjectionMatrix() const;

		/// <summary> Obtiene la matriz proyección de la cámara con el rango de profundidad normal. </summary>
		glm::mat4 GetProjectionMatrix_UnreversedZ() const;

		/// <summary> Obtiene la matriz view de la cámara. </summary>
		/// <param name="transform">Transform de la cámara.</param>
		glm::mat4 GetViewMatrix(const Transform3D& transform) const;

	private:

		float fov = 45.0f;
		float fovLimitDown = 1.0f;
		float fovLimitUp = 45.0f;

		float nearPlane = 0.001f;
		float farPlane = 50.0f;

		Vector2f angles = Vector2f::Zero;
		Vector2f accumulatedAngles = Vector2f::Zero;

		/// <summary>  Vector 'arriba' del mundo. { 0, 1, 0 }. </summary>
		const static Vector3f worldUpVector;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::ECS::CameraComponent3D>(const OSK::ECS::CameraComponent3D& data);

	template <>
	OSK::ECS::CameraComponent3D DeserializeJson<OSK::ECS::CameraComponent3D>(const nlohmann::json& json);

}
