#pragma once

#include "IGraphicsPipeline.h"

#include "OglTypes.h"
#include "PipelineCreateInfo.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL GraphicsPipelineOgl : public IGraphicsPipeline {

	public:

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, Format format, const VertexInfo& vertexInfo) override;

		OglPipelineHandler GetPipelineHandler() const;

	private:

		PipelineCreateInfo pipelineInfo{};

		OglPipelineHandler pipelineHandler = OGL_NULL_HANDLER;

	};

}
