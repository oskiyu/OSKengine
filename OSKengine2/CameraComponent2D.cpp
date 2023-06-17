#include "CameraComponent2D.h"

#include "Window.h"
#include "Transform2D.h"
#include "OSKengine.h"
#include "Window.h"
#include <glm/ext\matrix_clip_space.hpp>
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

void CameraComponent2D::LinkToDisplay(const IO::IDisplay* display) {
	this->display = display;
}

void CameraComponent2D::UnlinkDsiplay() {
	LinkToDisplay(nullptr);
}

glm::mat4 CameraComponent2D::GetProjection(const Transform2D& cameraTransform) const {
	return display
		? glm::ortho<float>(cameraTransform.GetPosition().x, (float)display->GetResolution().x, (float)display->GetResolution().y, cameraTransform.GetPosition().y, -1.0f, 1.0f)
		: glm::ortho<float>(cameraTransform.GetPosition().x, targetSize.x, cameraTransform.GetPosition().y, targetSize.y, -1.0f, 1.0f);
}

void CameraComponent2D::UpdateUniformBuffer(const Transform2D& cameraTransform) {
	uniformBuffer->MapMemory();
	uniformBuffer->Write(GetProjection(cameraTransform));
	uniformBuffer->Unmap();
}

GpuBuffer* CameraComponent2D::GetUniformBuffer() const {
	return uniformBuffer.GetPointer();
}

void CameraComponent2D::SetTargetSize(const Vector2f& size) {
	targetSize = size;
}

Vector2f CameraComponent2D::PointInWindowToPointInWorld(const Vector2f& point) const {
	Vector2f relative = Vector2f::Zero;
	relative.x = point.x / Engine::GetDisplay()->GetResolution().x;
	relative.y = point.y / Engine::GetDisplay()->GetResolution().y;

	return relative * targetSize;
}
