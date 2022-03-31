#include "CommandListOgl.h"

#include "IGpuImage.h"
#include "Color.hpp"
#include "RenderpassOgl.h"
#include "Assert.h"
#include "Viewport.h"
#include "GpuVertexBufferOgl.h"
#include "GpuIndexBufferOgl.h"
#include "GraphicsPipelineOgl.h"
#include "Material.h"

#include <glad/glad.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

void CommandListOgl::Reset() {

}

void CommandListOgl::Start() {

}

void CommandListOgl::Close() {

}

void CommandListOgl::TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next) {
	image->SetLayout(next);
}

void CommandListOgl::BeginRenderpass(IRenderpass* renderpass) {
	BeginAndClearRenderpass(renderpass, Color::BLACK());
}

void CommandListOgl::BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) {
	glBindFramebuffer(GL_FRAMEBUFFER, renderpass->As<RenderpassOgl>()->GetFramebuffer());
	glClearColor(color.Red, color.Green, color.Blue, color.Alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CommandListOgl::EndRenderpass(IRenderpass* renderpass) {
	glBindFramebuffer(GL_FRAMEBUFFER, OGL_NULL_HANDLER);
}

void CommandListOgl::BindMaterial(const Material* material) {
	glUseProgram(material->GetGraphicsPipeline(currentRenderpass)->As<GraphicsPipelineOgl>()->GetPipelineHandler());
}

void CommandListOgl::BindVertexBuffer(IGpuVertexBuffer* buffer) {
	glBindVertexArray(buffer->As<GpuVertexBufferOgl>()->GetViewHandler());
}

void CommandListOgl::BindIndexBuffer(IGpuIndexBuffer* buffer) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->As<GpuIndexBufferOgl>()->GetHandler());
}

void CommandListOgl::BindMaterialSlot(const IMaterialSlot* slot) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::DrawSingleInstance(TSize numIndices) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::DrawSingleMesh(TSize firstIndex, TSize numIndices) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::SetViewport(const Viewport& vp) {
	glViewport(
		vp.rectangle.GetRectanglePosition().X,
		vp.rectangle.GetRectanglePosition().Y,
		vp.rectangle.GetRectangleSize().X,
		vp.rectangle.GetRectangleSize().Y);
}

void CommandListOgl::SetScissor(const Vector4ui& scissor) {
	glScissor(
		scissor.GetRectanglePosition().X,
		scissor.GetRectanglePosition().Y,
		scissor.GetRectangleSize().X,
		scissor.GetRectangleSize().Y);
}
