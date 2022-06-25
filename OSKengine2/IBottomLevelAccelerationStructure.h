#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "IGpuDataBuffer.h"

#include <glm.hpp>

namespace OSK::GRAPHICS {

	class IGpuVertexBuffer;
	class IGpuIndexBuffer;

	/// <summary>
	/// Estructura de aceleraci�n de nivel bajo: contiene la geometr�a
	/// de un modelo 3D.
	/// </summary>
	/// 
	/// @todo Permitir usar meshes en vez de modelos enteros.
	/// @todo Implementaci�n en DirectX 12.
	class OSKAPI_CALL IBottomLevelAccelerationStructure {

	public:

		OSK_DEFINE_AS(IBottomLevelAccelerationStructure);

		/// <summary>
		/// Crea la estructura de aceleraci�n con los triangulos definidos
		/// por los v�rtices y los �ndices.
		/// </summary>
		/// 
		/// @todo Permitir determinar qu� v�rtices y qu� indices a usar, en vez de usar
		/// todos los v�rtices y todos los �ndices dados.
		virtual void Setup(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) = 0;

		/// <summary>
		/// Actualiza la estructura de aceleraci�n, para reflejar los cambios realizados en
		/// la matriz de modelo.
		/// </summary>
		/// 
		/// @pre Se debe haber creado la estructura, llamando a IBottomLevelAccelerationStructure::Setup.
		/// 
		/// @todo Implementaci�n DX12.
		virtual void Update() = 0;

		/// <summary>
		/// Establece la matriz de modelo de la geometr�a
		/// contenida dentro de la estructura.
		/// </summary>
		/// 
		/// @note Para poder aplicarse, se debe llamar despu�s a IBottomLevelAccelerationStructure::Update.
		void SetMatrix(const glm::mat4& matrix);

		/// <summary>
		/// True si se ha establecido la matriz pero no se ha llamado a Update.
		/// </summary>
		bool IsDirty() const;

	protected:

		UniquePtr<GpuDataBuffer> matrixBuffer;
		bool isDirty = false;

	};

}
