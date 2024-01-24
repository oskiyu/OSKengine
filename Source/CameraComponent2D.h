#pragma once

#include "OSKmacros.h"
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Component.h"
#include "UniquePtr.hpp"
#include "GpuBuffer.h"
#include <glm/glm.hpp>

#include "Serializer.h"

namespace OSK::IO {
	class IDisplay;
}

namespace OSK::ECS {

	class Transform2D;

	/// <summary>
	/// Cámara para un mundo de dos dimensiones.
	/// </summary>
	class OSKAPI_CALL CameraComponent2D {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

		OSK_COMPONENT("OSK::CameraComponent2D");

		CameraComponent2D();

		/// @brief Enlaza la cámara a un display, para que tenga el tamaño del display.
		/// 
		/// @pre display no debe ser null.
		/// @pre El puntero al display debe ser ESTABLE.
		void LinkToDisplay(const IO::IDisplay* display);

		/// <summary> Quita el enlace al display. </summary>
		void UnlinkDsiplay();

		/// <summary> Establece el tamaño del área a renderizar. </summary>
		/// 
		/// @warning Se ignorará si se ha enlazado a una ventana.
		void SetTargetSize(const Vector2f& targetSize);


		/// <summary> Devuelve la matriz proyección de la cámara. </summary>
		glm::mat4 GetProjection(const Transform2D& cameraTransform) const;

		/// <summary> Actualiza el buffer en la GPU. </summary>
		/// <param name="cameraTransform">Transform de la cámara.</param>
		/// 
		/// @note Debe llamarse cada frame que se haya modificado la cámara.
		void UpdateUniformBuffer(const Transform2D& cameraTransform);

		GRAPHICS::GpuBuffer* GetUniformBuffer() const;

		/// <summary> Transforma un punto 2D en la ventana al punto 2D al que apunta la cámara en el mundo 2D. </summary>
		/// <param name="point">Punto en coordenadas de la ventana.</param>
		/// <returns>Punto, en coordenadas del mundo 2D.</returns>
		Vector2f PointInWindowToPointInWorld(const Vector2f& point) const;

	private:

		const IO::IDisplay* m_display = nullptr;

		glm::mat4 m_projection;

		Vector2f m_targetSize;

		UniquePtr<GRAPHICS::GpuBuffer> m_uniformBuffer;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::ECS::CameraComponent2D>(const OSK::ECS::CameraComponent2D& data);

	template <>
	OSK::ECS::CameraComponent2D DeserializeJson<OSK::ECS::CameraComponent2D>(const nlohmann::json& json);

}
