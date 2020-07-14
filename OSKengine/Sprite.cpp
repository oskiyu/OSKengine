#include "Sprite.h"
#include <ext\matrix_transform.hpp>

namespace OSK {

	void Sprite::SetPosition(const Vector2& pos) {
		rectangle.X = pos.X;
		rectangle.Y = pos.Y;

		updateModel();
	}

	void Sprite::SetSize(const Vector2& size) {
		rectangle.Z = size.X;
		rectangle.W = size.Y;

		updateModel();
	}

	void Sprite::SetRectangle(const Vector4& rec) {
		rectangle = rec;

		updateModel();
	}

	Vector2 Sprite::GetPosition() const {
		return rectangle.GetRectanglePosition();
	}

	Vector2 Sprite::GetSize() const {
		return rectangle.GetRectangleSize();
	}

	Vector4 Sprite::GetRectangle() const {
		return rectangle;
	}

	void Sprite::updateModel() {
		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(rectangle.GetRectanglePosition().X, rectangle.GetRectanglePosition().Y, 0.0f));

		model = glm::translate(model, glm::vec3(0.5f * rectangle.GetRectangleSize().X, 0.5f * rectangle.GetRectangleSize().Y, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * rectangle.GetRectangleSize().X, -0.5f * rectangle.GetRectangleSize().Y, 0.0f));

		model = glm::scale(model, glm::vec3(rectangle.GetRectangleSize().X, rectangle.GetRectangleSize().Y, 1.0f));
	}

	PushConst2D Sprite::getPushConst() const {
		return PushConst2D{ model, color.ToGLM() };
	}

}