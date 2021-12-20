#pragma once

#include "ICommandList.h"

namespace OSK {

	class OSKAPI_CALL CommandListOgl : public ICommandList {

	public:

		void Reset() override;
		void Start() override;
		void Close() override;
		void TransitionImageLayout(GpuImage*, GpuImageLayout) override;

		void BeginRenderpass(IRenderpass* renderpass);
		void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color);
		void EndRenderpass(IRenderpass* renderpass);

	};

}
