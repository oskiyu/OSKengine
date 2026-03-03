#pragma once

#include "RenderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL ExampleComputePass : public IRenderPass {

	public:

		explicit ExampleComputePass(
			GdrBufferUuid bufferRef,
			GdrImageUuid imageRef);

		void Execute(ICommandList* cmdList) override;
		
		std::string_view GetName() const override {
			return "XD";
		}

	};

}
