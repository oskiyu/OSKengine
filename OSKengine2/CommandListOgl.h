#pragma once

#include "ICommandList.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL CommandListOgl : public ICommandList {

	public:

		/// @deprecated OpenGL no implementado.
		void Reset() override;
		/// @deprecated OpenGL no implementado.
		void Start() override;
		/// @deprecated OpenGL no implementado.
		void Close() override;
		/// @deprecated OpenGL no implementado.
		void TransitionImageLayout(GpuImage*, GpuImageLayout, GpuImageLayout, TSize, TSize) override;

		/// @deprecated OpenGL no implementado.
		void BeginRenderpass(RenderTarget* renderpass) override;
		/// @deprecated OpenGL no implementado.
		void BeginAndClearRenderpass(RenderTarget* renderpass, const Color& color) override;
		/// @deprecated OpenGL no implementado.
		void EndRenderpass(RenderTarget* renderpass) override;

		/// @deprecated OpenGL no implementado.
		void BindMaterial(const Material* material) override;
		/// @deprecated OpenGL no implementado.
		void BindVertexBuffer(const IGpuVertexBuffer* buffer) override;
		/// @deprecated OpenGL no implementado.
		void BindIndexBuffer(const IGpuIndexBuffer* buffer) override;
		/// @deprecated OpenGL no implementado.
		void BindMaterialSlot(const IMaterialSlot* slot) override;
		/// @deprecated OpenGL no implementado.
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) override;

		/// @deprecated OpenGL no implementado.
		void DrawSingleInstance(TSize numIndices) override;
		/// @deprecated OpenGL no implementado.
		void DrawSingleMesh(TSize firstIndex, TSize numIndices) override;
		/// @deprecated OpenGL no implementado.
		void TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) override;

		/// @deprecated OpenGL no implementado.
		void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) override;

		/// @deprecated OpenGL no implementado.
		void SetViewport(const Viewport& viewport) override;
		/// @deprecated OpenGL no implementado.
		void SetScissor(const Vector4ui& scissor) override;

	};

}
