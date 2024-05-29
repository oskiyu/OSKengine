#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

namespace OSK::GRAPHICS {

	class MaterialLayout;

	/// <summary>
	/// Un pipeline layout define el layout del pipeline, es decir:
	/// define qué recursos necesita para el renderizado (textura, buffers,
	/// push constants...).
	/// 
	/// Representación nativa de MaterialLayout.
	/// 
	/// @see MaterialLayout.
	/// </summary>
	class OSKAPI_CALL IPipelineLayout {

	public:

		/// @brief 
		/// @param layout 
		/// 
		/// @throws PipelineLayoutCreationException Si hay algún problema con el API nativo.
		IPipelineLayout(const MaterialLayout* layout);
		virtual ~IPipelineLayout() = default;

		OSK_DEFINE_AS(IPipelineLayout);

	};

}
