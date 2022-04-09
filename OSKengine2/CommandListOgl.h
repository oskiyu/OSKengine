#pragma once

#include "ICommandList.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL CommandListOgl : public ICommandList {

	public:

		void Reset() override;
		void Start() override;
		void Close() override;
		void TransitionImageLayout(GpuImage*, GpuImageLayout, GpuImageLayout, TSize, TSize) override;

		void BeginRenderpass(IRenderpass* renderpass) override;
		void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) override;
		void EndRenderpass(IRenderpass* renderpass) override;

		void BindMaterial(const Material* material) override;
		void BindVertexBuffer(IGpuVertexBuffer* buffer) override;
		void BindIndexBuffer(IGpuIndexBuffer* buffer) override;
		void BindMaterialSlot(const IMaterialSlot* slot) override;
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) override;

		void DrawSingleInstance(TSize numIndices) override;
		void DrawSingleMesh(TSize firstIndex, TSize numIndices) override;

		void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) override;

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Vector4ui& scissor) override;

	};

}
