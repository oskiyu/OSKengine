#pragma once

#include "OSKmacros.h"
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Component.h"
#include "UniquePtr.hpp"
#include "IGpuUniformBuffer.h"
#include <glm.hpp>

namespace OSK::IO {
	class Window;
}

namespace OSK::ECS {

	class Transform2D;

	/// <summary>
	/// C�mara para un mundo de dos dimensiones.
	/// </summary>
	class OSKAPI_CALL CameraComponent2D {

	public:

		OSK_COMPONENT("OSK::CameraComponent2D");

		CameraComponent2D();

		/// <summary>
		/// Enlaza la c�mara a una ventana, para que tenga el tama�o de la ventana.
		/// El puntero a la ventana debe ser ESTABLE.
		/// </summary>
		void LinkToWindow(const IO::Window* window);

		/// <summary>
		/// Quita el enlace a la ventana.
		/// </summary>
		void UnlinkWindow();

		/// <summary>
		/// Establece el tama�o del �rea a renderizar.
		/// </summary>
		void SetTargetSize(const Vector2f& targetSize);

		/// <summary>
		/// Devuelve la matriz proyecci�n de la c�mara.
		/// </summary>
		glm::mat4 GetProjection(const Transform2D& cameraTransform) const;

		/// <summary>
		/// Actualiza el buffer en la GPU.
		/// Debe llamarse cada frame.
		/// </summary>
		/// <param name="cameraTransform">Transform de la c�mara.</param>
		void UpdateUniformBuffer(const Transform2D& cameraTransform);

		GRAPHICS::IGpuUniformBuffer* GetUniformBuffer() const;

		/// <summary>
		/// Transforma un punto 2D en la ventana al punto 2D al que apunta la c�mara en el mundo 2D.
		/// </summary>
		/// <param name="point">Punto en coordenadas de la ventana.</param>
		/// <returns>Punto, en coordenadas del mundo 2D.</returns>
		Vector2f PointInWindowToPointInWorld(const Vector2& point) const;

	private:

		/// <summary>
		/// Ventana.
		/// </summary>
		const IO::Window* window = nullptr;

		/// <summary>
		/// Matriz de la c�mara.
		/// </summary>
		glm::mat4 projection;

		Vector2f targetSize;

		UniquePtr<GRAPHICS::IGpuUniformBuffer> uniformBuffer;

	};

}