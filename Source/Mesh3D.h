#pragma once

#include "OSKmacros.h"

#include "SphereCollider.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un mesh representa una serie de primitivas (triángulos) de un
	/// modelo 3D que comparten unas mismas propiedades de material.
	/// </summary>
	/// 
	/// @note Los buffers con vértices e índices son propiedad del modelo 3D.
	/// 
	/// @note Un mesh únicamente almacena los índices que le corresponden,
	/// representados por el ID del primer índice y el número de índices.
	class OSKAPI_CALL Mesh3D {

	public:

		/// <summary>
		/// Crea el mesh que será compuesto por los los triángulos
		/// compuestos con los índices dados.
		/// </summary>
		/// 
		/// @pre El número de índices debe ser múltipo de 3.
		/// @pre El número de índices debe ser mayor que 0.
		Mesh3D(
			USize32 numIndices, 
			USize32 firstIndex, 
			const Vector3f& sphereCenter,
			UIndex64 id);


		/// @return Identificador único del mesh.
		UIndex64 GetMeshId() const;


		/// @brief Actualiza la esfera del mesh.
		/// @param radius Radio.
		void SetBoundingSphereRadius(float radius);

		/// @return Esfera que contiene el mesh.
		const COLLISION::SphereCollider& GetBounds() const;

		/// @return Esfera que contiene el mesh.
		COLLISION::SphereCollider& GetBounds();

		/// @return Offset del mesh respecto al modelo 3D.
		const Vector3f& GetSphereCenter() const;


		/// <summary>
		/// Devuelve el número de índices que componen la malla de
		/// este mesh.
		/// </summary>
		/// 
		/// @note Mútliplo de 3.
		USize32 GetNumberOfIndices() const;

		/// <summary>
		/// Offset del primer índice respecto al buffer de índices
		/// del modelo 3D al que pertenece este mesh.
		/// </summary>
		UIndex32 GetFirstIndexId() const;

	private:

		UIndex64 m_meshId = 0;

		USize32 numIndices = 0;
		UIndex32 firstIndex = 0;
		
		Vector3f sphereCenter = Vector3f::Zero;

		COLLISION::SphereCollider sphere{};

	};

}
