#include "CameraComponent2D.h"

#include "Window.h"
#include "TransformComponent2D.h"
#include "OSKengine.h"
#include "Window.h"
#include <glm/ext\matrix_clip_space.hpp>
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

#include "Math.h"


using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

CameraComponent2D::CameraComponent2D() {
	m_uniformBuffer = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4));
}

void CameraComponent2D::LinkToDisplay(const IO::IDisplay* display) {
	m_display = display;
}

void CameraComponent2D::UnlinkDsiplay() {
	LinkToDisplay(nullptr);
}

glm::mat4 CameraComponent2D::GetProjection(const Transform2D& cameraTransform) const {
	return m_display
		? glm::ortho<float>(cameraTransform.GetPosition().x, (float)m_display->GetResolution().x, (float)m_display->GetResolution().y, cameraTransform.GetPosition().y, -1.0f, 1.0f)
		: glm::ortho<float>(cameraTransform.GetPosition().x, m_targetSize.x, cameraTransform.GetPosition().y, m_targetSize.y, -1.0f, 1.0f);
}

void CameraComponent2D::UpdateUniformBuffer(const Transform2D& cameraTransform) {
	m_uniformBuffer->MapMemory();
	m_uniformBuffer->Write(GetProjection(cameraTransform));
	m_uniformBuffer->Unmap();
}

const GpuBuffer* CameraComponent2D::GetUniformBuffer() const {
	return m_uniformBuffer.GetPointer();
}

void CameraComponent2D::SetTargetSize(const Vector2f& size) {
	m_targetSize = size;
}

Vector2f CameraComponent2D::PointInWindowToPointInWorld(const Vector2f& point) const {
	Vector2f relative = Vector2f::Zero;
	relative.x = point.x / Engine::GetDisplay()->GetResolution().x;
	relative.y = point.y / Engine::GetDisplay()->GetResolution().y;

	return relative * m_targetSize;
}



template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::CameraComponent2D>(const OSK::ECS::CameraComponent2D& data) {
	nlohmann::json output{};

	output["m_projection"] = SerializeData<glm::mat4>(data.m_projection);

	return output;
}

template <>
OSK::ECS::CameraComponent2D PERSISTENCE::DeserializeComponent<OSK::ECS::CameraComponent2D>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	OSK::ECS::CameraComponent2D output{};

	output.m_projection = DeserializeData<glm::mat4>(json["m_projection"]);

	return output;
}

template <>
PERSISTENCE::BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::CameraComponent2D>(const OSK::ECS::CameraComponent2D& data) {
	PERSISTENCE::BinaryBlock output{};

	output.AppendBlock(BinarySerializeData<glm::mat4>(data.m_projection));

	return output;
}

template <>
OSK::ECS::CameraComponent2D PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::CameraComponent2D>(PERSISTENCE::BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	OSK::ECS::CameraComponent2D output{};

	output.m_projection = BinaryDeserializeData<glm::mat4>(reader);
	
	return output;
}
