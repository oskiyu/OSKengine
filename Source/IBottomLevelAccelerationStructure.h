#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "UniquePtr.hpp"
#include "GpuBuffer.h"
#include "RtAccelerationStructureFlags.h"

#include <glm/glm.hpp>
#include "VertexBufferView.h"
#include "IndexBufferView.h"

namespace OSK::GRAPHICS {

	class ICommandList;

	/// @brief Estructura de aceleración de nivel bajo: contiene la geometría
	/// de un modelo 3D.
	/// 
	/// @todo Permitir usar meshes en vez de modelos enteros.
	/// @todo Implementación en DirectX 12.
	class OSKAPI_CALL IBottomLevelAccelerationStructure {

	public:

		virtual ~IBottomLevelAccelerationStructure() = default;

		OSK_DEFINE_AS(IBottomLevelAccelerationStructure);

		/// @brief Crea la estructura de aceleración con los triangulos definidos
		/// por los vértices y los índices.
		/// 
		/// @param vertexBuffer 
		/// @param indexBuffer 
		/// @param flags Configuración extra.
		/// 
		/// @todo Permitir determinar qué vértices y qué indices a usar, en vez de usar
		/// todos los vértices y todos los índices dados.
		virtual void Setup(
			const GpuBuffer& vertexBuffer, 
			const VertexBufferView& vertexView,
			const GpuBuffer& indexBuffer, 
			const IndexBufferView& indexView,
			RtAccelerationStructureFlags flags) = 0;

		/// <summary>
		/// Actualiza la estructura de aceleración, para reflejar los cambios realizados en
		/// la matriz de modelo.
		/// </summary>
		/// 
		/// @pre Se debe haber creado la estructura, llamando a IBottomLevelAccelerationStructure::Setup.
		/// 
		/// @todo Implementación DX12.
		virtual void Update(ICommandList* cmdList) = 0;

		/// <summary>
		/// Establece la matriz de modelo de la geometría
		/// contenida dentro de la estructura.
		/// </summary>
		/// 
		/// @note Para poder aplicarse, se debe llamar después a IBottomLevelAccelerationStructure::Update.
		/// 
		/// @throws MatrixBufferNotCreatedException Si no se creó correctamente el buffer de matriz.
		void SetMatrix(const glm::mat4& matrix);

		/// <summary>
		/// True si se ha establecido la matriz pero no se ha llamado a Update.
		/// </summary>
		bool IsDirty() const;

	protected:

		UniquePtr<GpuBuffer> matrixBuffer;
		bool isDirty = false;

	};

}
