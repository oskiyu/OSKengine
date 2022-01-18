#pragma once

#include "OSKmacros.h"

namespace OSK {

	class IGpu;
	struct PipelineCreateInfo;

	/// <summary>
	/// El graphics pipeline es el encargado de transformar los modelos 3D en una imagen final.
	/// Contiene información y ajustes que cambian este proceso de renerizado.
	/// Debe usarse con shaders para que funcione.
	/// </summary>
	class OSKAPI_CALL IGraphicsPipeline {

	public:

		virtual void Create(IGpu* device, const PipelineCreateInfo& info) = 0;

		template <typename T> T* As() const requires std::is_base_of_v<IGraphicsPipeline, T> {
			return (T*)this;
		}

	private:

	};

}
