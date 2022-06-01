#pragma once

#include "OSKmacros.h"

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

		IPipelineLayout(const MaterialLayout* layout);
		virtual ~IPipelineLayout() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IPipelineLayout, T> {
			return (T*)this;
		}

	};

}
