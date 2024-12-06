#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"
#include "IPipelineLayout.h"
#include "Format.h"
#include "IGpuObject.h"

namespace OSK::GRAPHICS {

	class MaterialLayout;
	class IGpu;
	struct PipelineCreateInfo;
	class VertexInfo;


	class OSKAPI_CALL IMeshPipeline : public IGpuObject {

	public:

		virtual ~IMeshPipeline() = default;

		OSK_DEFINE_AS(IMeshPipeline);

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre algún error durante la compilación de los shaders.
		virtual void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info) = 0;


		/// @brief Devuelve el layout usado por este pipeline.
		/// @pre Se debe haber creado correctamente el pipeline.
		/// @note Si no se ha creado correctamente el pipeline, devolverá null.
		const IPipelineLayout* GetLayout() const;

	protected:

		UniquePtr<IPipelineLayout> m_layout;

	};

}
