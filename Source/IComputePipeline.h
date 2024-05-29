#pragma once

#include "ApiCall.h"
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
		const IPipelineLayout* GetLayout() const;

	protected:

		UniquePtr<IPipelineLayout> layout;

	};

}
