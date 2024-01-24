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
	/// C�mara para un mundo tridimensional.
	/// </summary>
	class OSKAPI_CALL CameraComponent3D {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

		OSK_COMPONENT("OSK::CameraComponent3D");

		/// <summary> FoV de la c�mara. </summary>
		/// 
		/// @note Zoom de la c�mara.
		float GetFov() const;

		/// <summary> Establece el FoV de la c�mara, si est� en los l�mites de FoV. </summary>
		/// 
		/// @note Si el valor est� fuera de los l�mites, se pondr� el valor v�lido
		/// m�s cercano.
		void SetFov(float fov);

		/// <summary> A�ade FoV a la c�mara. </summary>
		/// <param name="diff">FoV delta.</param>
		/// 
		/// @note Si el valor est� fuera de los l�mites, se pondr� el valor v�lido
		void AddFov(float diff);

		/// <summary>  Establece los l�mites m�nimos y m�ximos de field of view. </summary>
		/// 
		/// @pre Max debe ser > min.
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
		/// @pre Max debe ser > min.
		/// @throws std::runtime_exception si near >= far.
		void SetPlanes(float near, float far);


		/// <summary> Rota la c�mara. </summary>
		/// 
		/// <param name="angleX">Yaw.</param>
		/// <param name="angleY">Pitch.</param>
		void Rotate(float angleX, float angleY);
		
		/// <summary>
		/// Actualiza el transform de la c�mara, para que tenga
		/// su rotaci�n.
		/// </summary>
		/// 
		/// @pre transform no debe ser null.
		void UpdateTransform(Transform3D* transform);


		/// @param transform Transform de la c�mara.
		/// @return Frustum de la c�mara.
		AnyFrustum GetFrustum(const Transform3D& transform) const;


		/// @return Matriz proyecci�n de la c�mara, con depth Z invertido y
		/// far-plane infinito.
		glm::mat4 GetProjectionMatrix() const;

		/// <summary> Obtiene la matriz proyecci�n de la c�mara con el rango de profundidad normal. </summary>
		glm::mat4 GetProjectionMatrix_UnreversedZ() const;

		/// <summary> Obtiene la matriz view de la c�mara. </summary>
		/// <param name="transform">Transform de la c�mara.</param>
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
