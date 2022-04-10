#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"

namespace OSK::GRAPHICS {

	class MaterialLayout;
	class IGpu;
	struct PipelineCreateInfo;
	class IPipelineLayout;
	class VertexInfo;

	/// <summary>
	/// El graphics pipeline es el encargado de transformar los modelos 3D en una imagen final.
	/// Contiene información y ajustes que cambian este proceso de renerizado.
	/// Debe usarse con shaders para que funcione.
	/// </summary>
	class OSKAPI_CALL IGraphicsPipeline {

	public:

		virtual ~IGraphicsPipeline() = default;

		/// <summary>
		/// Crea el pipeline con la configuración dada.
		/// </summary>
		/// <param name="info">Configuración del pipeline.</param>
		virtual void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) = 0;

		template <typename T> T* As() const requires std::is_base_of_v<IGraphicsPipeline, T> {
			return (T*)this;
		}

		IPipelineLayout* GetLayout() const;

	protected:

		UniquePtr<IPipelineLayout> layout;

	};

}
