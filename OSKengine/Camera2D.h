#pragma once

#include "WindowAPI.h"
#include "Vector2.hpp"
#include "Transform2D.h"
#include <glm.hpp>

#include "UniformBuffer.h"
#include "MaterialInstance.h"

namespace OSK {

	struct UboCamera2D {
		alignas(16) glm::mat4 cameraMatrix;
	};

	/// <summary>
	/// C�mara para un mundo de dos dimensiones.
	/// </summary>
	class OSKAPI_CALL Camera2D {

		friend class RenderAPI;
		friend class SpriteBatch;

	public:

		/// <summary>
		/// Crea una c�mara, para esta ventana.
		/// </summary>
		/// <param name="window">Ventana renderizada.</param>
		Camera2D(WindowAPI* window);

		~Camera2D();

		/// <summary>
		/// Actualiza los valores de la c�mara.
		/// </summary>
		void Update();

		/// <summary>
		/// Transform de la c�mara.
		/// </summary>
		Transform2D& GetTransform();

		/// <summary>
		/// Devuelve la matriz proyecci�n de la c�mara.
		/// </summary>
		glm::mat4 GetProjection() const;

		/// <summary>
		/// TRUE: cambiar el tama�o de la ventana no cambia el �rea de la c�mara.
		/// FALSE: cambiar el tama�o de la ventana s� cambia el �rea de la c�mara.
		/// </summary>
		bool useTargetSize = false;

		/// <summary>
		/// Transforma un punto 2D en la ventana al punto 2D al que apunta la c�mara en el mundo 2D.
		/// </summary>
		/// <param name="point">Punto en coordenadas de la ventana.</param>
		/// <returns>Punto, en coordenadas del mundo 2D.</returns>
		Vector2 PointInWindowToPointInWorld(const Vector2& point) const;

		/// <summary>
		/// �rea de la c�mara.
		/// </summary>
		Vector2 targetSize{};

		SharedPtr<UniformBuffer> GetUniformBuffer() {
			return cameraBuffer;
		}

		MaterialInstance* GetCameraMaterial() const {
			return cameraMaterial.GetPointer();
		}

	private:

		/// <summary>
		/// Ventana.
		/// </summary>
		WindowAPI* window = nullptr;

		/// <summary>
		/// Transform de la c�mara.
		/// </summary>
		Transform2D transform;

		/// <summary>
		/// Matriz de la c�mara.
		/// </summary>
		glm::mat4 projection;

		SharedPtr<UniformBuffer> cameraBuffer = new UniformBuffer;
		OwnedPtr<MaterialInstance> cameraMaterial = nullptr;

	};

}