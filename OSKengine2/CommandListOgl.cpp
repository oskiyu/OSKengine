#include "CommandListOgl.h"

#include "IGpuImage.h"
#include "Color.hpp"
#include "RenderpassOgl.h"

#include <glad/glad.h>

using namespace OSK;

void CommandListOgl::Reset() {

}

void CommandListOgl::Start() {

}

void CommandListOgl::Close() {

}

void CommandListOgl::TransitionImageLayout(GpuImage* image, GpuImageLayout next) {
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
