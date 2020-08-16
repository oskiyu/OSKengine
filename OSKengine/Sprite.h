#pragma once

#include "VulkanRenderizableObject.h"

#include <array>

#include <glm.hpp>
#include "Transform2D.h"
#include "Color.h"

namespace OSK {

	//Push constants de un sprite.
	struct OSKAPI_CALL PushConst2D {
		glm::mat4 model;
		glm::vec4 color = glm::vec4(1.0f);
	};

	//Representa un sprite, una textura con un transform.
	struct OSKAPI_CALL Sprite {
		friend class VulkanRenderer;

	public:

		//Establece la región de la textura que se renderizará en este sprite.
		void SetTexCoords(const Vector4& texCoords);

		//Establece la región de la textura que se renderizará en este sprite.
		void SetTexCoords(const float& x, const float& y, const float& width, const float& hegith);

		//Transform del sprite.
		Transform2D SpriteTransform;

		//Textura que se renderiza en este sprite.
		Texture* texture;

		//Color del sprite.
		Color color = Color(1.0f);

	private:
		PushConst2D getPushConst();

		glm::mat4 model = glm::mat4(1);
		
		Vector4 rectangle{};

		float_t rotation = 0.0f;

		VulkanBuffer VertexBuffer;
		VulkanBuffer IndexBuffer;

		std::vector<Vertex> Vertices = {
			{{0, 0, 0}, {1.0f, 1.0f, 1.0f}, {0, 0}},
			{{1, 0, 0}, {1.0f, 1.0f, 1.0f}, {1, 0}},
			{{1, 1, 0}, {1.0f, 1.0f, 1.0f}, {1, 1}},
			{{0, 1, 0}, {1.0f, 1.0f, 1.0f}, {0, 1}}
		};

		std::array<uint16_t, 6> Indices = {
			0, 1, 2, 2, 3, 0
		};

		bool hasChanged = false;
	};

}