#include "CameraComponent2D.h"

#include "Window.h"
#include "Transform2D.h"
#include "OSKengine.h"
#include "Window.h"
#include <ext\matrix_clip_space.hpp>
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

CameraComponent2D::CameraComponent2D() {
	uniformBuffer = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4)).GetPointer();
}

void CameraComponent2D::LinkToWindow(const Window* window) {
	this->window = window;
}

void CameraComponent2D::UnlinkWindow() {
	LinkToWindow(nullptr);
}

glm::mat4 CameraComponent2D::GetProjection(const Transform2D& cameraTransform) const {
	return window
		? glm::ortho<float>(cameraTransform.GetPosition().X, (float)window->GetWindowSize().X, (float)window->GetWindowSize().Y, cameraTransform.GetPosition().Y, -1.0f, 1.0f)
		: glm::ortho<float>(cameraTransform.GetPosition().X, targetSize.X, cameraTransform.GetPosition().Y, targetSize.Y, -1.0f, 1.0f);
}

void CameraComponent2D::UpdateUniformBuffer(const Transform2D& cameraTransform) {
	uniformBuffer->MapMemory();
	uniformBuffer->Write(GetProjection(cameraTransform));
	uniformBuffer->Unmap();
}

IGpuUniformBuffer* CameraComponent2D::GetUniformBuffer() const {
	return uniformBuffer.GetPointer();
}

void CameraComponent2D::SetTargetSize(const Vector2f& size) {
	targetSize = size;
}

Vector2f CameraComponent2D::PointInWindowToPointInWorld(const Vector2f& point) const {
	Vector2 relative = Vector2(0);
	relative.X = point.X / Engine::GetWindow()->GetWindowSize().X;
	relative.Y = point.Y / Engine::GetWindow()->GetWindowSize().Y;

	return relative * targetSize;
}
