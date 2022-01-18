#include "CommandListOgl.h"

#include "IGpuImage.h"
#include "Color.hpp"
#include "RenderpassOgl.h"
#include "Assert.h"
#include "Viewport.h"

#include <glad/glad.h>

using namespace OSK;

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
	glBindFramebuffer(GL_FRAMEBUFFER, oglNullHandler);
}

void CommandListOgl::BindPipeline(IGraphicsPipeline* pipeline) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::BindVertexBuffer(IGpuVertexBuffer* buffer) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListOgl::BindIndexBuffer(IGpuIndexBuffer* buffer) {
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
