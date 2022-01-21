#pragma once

#include "IGraphicsPipeline.h"

#include "OglTypes.h"
#include "PipelineCreateInfo.h"

namespace OSK {

	class OSKAPI_CALL GraphicsPipelineOgl : public IGraphicsPipeline {

	public:

		void Create(IGpu* device, const PipelineCreateInfo& info) override;

		OglPipelineHandler GetPipelineHandler() const;

	private:

		PipelineCreateInfo pipelineInfo{};

		OglPipelineHandler pipelineHandler = OGL_NULL_HANDLER;

	};

}
