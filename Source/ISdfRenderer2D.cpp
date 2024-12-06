#include "ISdfRenderer2D.h"

#include "CameraComponent2D.h"
#include "GpuImageViewConfig.h"
#include "SdfStringInfo.h"
#include "Font.h"
#include "EntityComponentSystem.h"

OSK::GRAPHICS::ISdfRenderer2D::ISdfRenderer2D(ECS::EntityComponentSystem* ecs) : m_ecs(ecs) {

}

void OSK::GRAPHICS::ISdfRenderer2D::Begin(ICommandList* commandList) {
	m_targetCommandList = commandList;
}

void OSK::GRAPHICS::ISdfRenderer2D::End() {
	m_targetCommandList = nullptr;
}

OSK::DynamicArray<OSK::GRAPHICS::SdfDrawCall2D> OSK::GRAPHICS::ISdfRenderer2D::GetTextDrawCalls(const SdfStringInfo& textInfo) {
	DynamicArray<SdfDrawCall2D> output = DynamicArray<SdfDrawCall2D>::CreateReserved(textInfo.text.size());

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const IGpuImageView* fontView = textInfo.font->image->GetView(viewConfig);

	const Vector2f originalPosition = textInfo.transform.GetPosition();
	float currentX = textInfo.transform.GetPosition().x;
	float currentY = textInfo.transform.GetPosition().y;

	for (const char c : textInfo.text) {
		const auto& character = textInfo.font->characters.at(c);

		if (c == '\n') {
			currentY += character.size.y + character.bearing.y;
			currentX = originalPosition.x;

			continue;
		}

		if (c == '\t') {
			currentX += (character.advance >> 6) * 4; // Font::SPACES_PER_TAB;

			continue;
		}

		if (c == ' ') {
			currentX += (character.advance >> 6);

			continue;
		}

		const float sizeX = character.size.x;
		const float sizeY = character.size.y;

		const float posX = currentX + character.bearing.x;
		const float posY = currentY - (character.bearing.y);


		SdfDrawCall2D drawCall{};
		drawCall.shape = SdfShape2D::RECTANGLE;
		drawCall.contentType = SdfDrawCallContentType2D::TEXTURE;
		drawCall.texture = fontView;
		drawCall.textureCoordinates = TextureCoordinates2D::Pixels(textInfo.font->characters.at(c).texCoords.ToVector4f());
		drawCall.fill = true;
		drawCall.mainColor = textInfo.color;

		drawCall.transform = ECS::Transform2D(ECS::EMPTY_GAME_OBJECT);
		drawCall.transform.SetScale(Vector2f(sizeX, sizeY) * textInfo.transform.GetScale());
		drawCall.transform.SetPosition({ posX, posY });
		drawCall.transform.SetRotation(textInfo.transform.GetRotation());

		output.Insert(drawCall);

		currentX += static_cast<float>(character.advance >> 6);
	}

	return output;
}

void OSK::GRAPHICS::ISdfRenderer2D::SetMaterial(Material* material) {
	m_material = material;
}

void OSK::GRAPHICS::ISdfRenderer2D::SetCamera(const ECS::CameraComponent2D& camera, const ECS::Transform2D& cameraTransform) {
	m_currentCameraMatrix = camera.GetProjection(cameraTransform);
}

void OSK::GRAPHICS::ISdfRenderer2D::SetCamera(ECS::GameObjectIndex gameObject) {
	SetCamera(
		m_ecs->GetComponent<OSK::ECS::CameraComponent2D>(gameObject),
		m_ecs->GetComponent<OSK::ECS::Transform2D>(gameObject));
}


glm::mat4 OSK::GRAPHICS::ISdfRenderer2D::GetCurrentCameraMatrix() const {
	return m_currentCameraMatrix;
}

OSK::GRAPHICS::ICommandList* OSK::GRAPHICS::ISdfRenderer2D::GetCurrentCommandList() {
	return m_targetCommandList;
}

OSK::GRAPHICS::Material* OSK::GRAPHICS::ISdfRenderer2D::GetCurrentMaterial() {
	return m_material;
}
