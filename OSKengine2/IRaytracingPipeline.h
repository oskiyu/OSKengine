#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IPipelineLayout.h"
#include "IRtShaderTable.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Pipeline para el renderizado usando trazado de rayos.
	/// </summary>
	/// 
	/// @pre Se debe haber iniciado el renderizador con el ray tracing activado.
	/// 
	/// @todo Implementación en DX12.
	class OSKAPI_CALL IRaytracingPipeline {

	public:

		virtual ~IRaytracingPipeline() = default;

		OSK_DEFINE_AS(IRaytracingPipeline);
		
		/// <summary>
		/// Devuelve el layout usado por este pipeline.
		/// </summary>
		/// 
		/// @pre Se debe haber creado correctamente el pipeline.
		/// @note Si no se ha creado correctamente el pipeline, devolverá null.
		const IPipelineLayout* GetLayout() const;

		/// <summary>
		/// Devuelve la tabla de shaders que usará el trazado de rayos.
		/// </summary>
		/// 
		/// @pre Se debe haber creado correctamente el pipeline.
		/// @note Si no se ha creado correctamente el pipeline, devolverá null.
		const IRtShaderTable* GetShaderTable() const;

	protected:

		UniquePtr<IPipelineLayout> layout;
		UniquePtr<IRtShaderTable> shaderTable;

	};

}
