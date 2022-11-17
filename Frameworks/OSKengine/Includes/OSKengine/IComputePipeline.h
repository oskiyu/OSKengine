#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IPipelineLayout.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL IComputePipeline {

	public:

		virtual ~IComputePipeline() = default;

		OSK_DEFINE_AS(IComputePipeline);

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
