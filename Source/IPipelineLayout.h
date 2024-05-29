#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

namespace OSK::GRAPHICS {

	class MaterialLayout;

	/// <summary>
	/// Un pipeline layout define el layout del pipeline, es decir:
	/// define qu� recursos necesita para el renderizado (textura, buffers,
	/// push constants...).
	/// 
	/// Representaci�n nativa de MaterialLayout.
	/// 
	/// @see MaterialLayout.
	/// </summary>
	class OSKAPI_CALL IPipelineLayout {

	public:

		/// @brief 
		/// @param layout 
		/// 
		/// @throws PipelineLayoutCreationException Si hay alg�n problema con el API nativo.
		IPipelineLayout(const MaterialLayout* layout);
		virtual ~IPipelineLayout() = default;

		OSK_DEFINE_AS(IPipelineLayout);

	};

}
