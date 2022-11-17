#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IPipelineLayout.h"
#include "Format.h"
#include "IGpuObject.h"

namespace OSK::GRAPHICS {

	class MaterialLayout;
	class IGpu;
	struct PipelineCreateInfo;
	class VertexInfo;

	/// <summary>
	/// El graphics pipeline es el encargado de transformar los modelos 3D en una imagen final.
	/// Contiene información y ajustes que cambian este proceso de renerizado.
	/// Debe usarse con shaders para que funcione.
	/// </summary>
	class OSKAPI_CALL IGraphicsPipeline : public IGpuObject {

	public:

		virtual ~IGraphicsPipeline() = default;

		/// <summary>
		/// Crea el pipeline con la configuración dada.
		/// </summary>
		/// <param name="info">Configuración del pipeline.</param>
		virtual void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) = 0;

		OSK_DEFINE_AS(IGraphicsPipeline);

		/// <summary>
		/// Devuelve el layout usado por este pipeline.
		/// </summary>
		/// 
		/// @pre Se debe haber creado correctamente el pipeline.
		/// @note Si no se ha creado correctamente el pipeline, devolverá null.
		IPipelineLayout* GetLayout() const;

	protected:

		UniquePtr<IPipelineLayout> layout;

	};

}
