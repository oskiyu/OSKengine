#pragma once

#include "VulkanRenderizableObject.h"

#include <array>

#include <glm.hpp>
#include "Vector2.h"
#include "Vector4.h"
#include "Color.h"

namespace OSK {

	struct OSKAPI_CALL PushConst2D {
		glm::mat4 model;
		glm::vec4 color = glm::vec4(1.0f);
	};

	struct OSKAPI_CALL Sprite {
		friend class VulkanRenderer;

	public:
		void SetPosition(const Vector2& pos);
		void SetSize(const Vector2& size);
		void SetRectangle(const Vector4& rec);

		Vector2 GetPosition() const;
		Vector2 GetSize() const;
		Vector4 GetRectangle() const;

		Texture* texture;

		Color color = Color(1.0f);
	private:
		void updateModel();

		PushConst2D getPushConst() const;

		glm::mat4 model = glm::mat4(1);
		
		Vector4 rectangle{};

		float_t rotation = 0.0f;

		VULKAN::VulkanBuffer VertexBuffer;
		VULKAN::VulkanBuffer IndexBuffer;

		std::vector<Vertex> Vertices = {
			{{0, 0, 0}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{1, 0, 0}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{1, 1, 0}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{0, 1, 0}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};

		std::array<uint16_t, 6> Indices = {
			0, 1, 2, 2, 3, 0
		};

		bool hasChanged = false;
	};

}