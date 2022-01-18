#pragma once

#include "ICommandList.h"

namespace OSK {

	class OSKAPI_CALL CommandListOgl : public ICommandList {

	public:

		void Reset() override;
		void Start() override;
		void Close() override;
		void TransitionImageLayout(GpuImage*, GpuImageLayout, GpuImageLayout) override;

		void BeginRenderpass(IRenderpass* renderpass);
		void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color);
		void EndRenderpass(IRenderpass* renderpass);

		void BindPipeline(IGraphicsPipeline* pipeline) override;
		void BindVertexBuffer(IGpuVertexBuffer* buffer) override;
		void BindIndexBuffer(IGpuIndexBuffer* buffer) override;

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Vector4ui& scissor) override;

	};

}
