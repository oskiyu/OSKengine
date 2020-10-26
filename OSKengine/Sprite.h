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
	class OSKAPI_CALL Sprite {
		friend class RenderAPI;
		friend class ContentManager;
		friend class SpriteBatch;

	public:

		~Sprite();

		//Establece la región de la textura que se renderizará en este sprite.
		void SetTexCoords(const Vector4& texCoords);

		void SetTexCoordsInPercent(const Vector4f& texCoords);

		//Establece la región de la textura que se renderizará en este sprite.
		void SetTexCoords(const float& x, const float& y, const float& width, const float& hegith);

		//Transform del sprite.
		Transform2D SpriteTransform;

		//Textura que se renderiza en este sprite.
		Texture* texture = nullptr;

		//Color del sprite.
		Color color = Color(1.0f);

	private:

		bool isOutOfScreen = false;

		PushConst2D getPushConst();

		glm::mat4 model = glm::mat4(1);
		
		Vector4 rectangle{};

		float_t rotation = 0.0f;

		VulkanBuffer VertexBuffer;

		static VulkanBuffer IndexBuffer;

		std::vector<Vertex> Vertices = {
			{{0, 0, 0}, {1.0f, 1.0f, 1.0f}, {0, 0}},
			{{1, 0, 0}, {1.0f, 1.0f, 1.0f}, {1, 0}},
			{{1, 1, 0}, {1.0f, 1.0f, 1.0f}, {1, 1}},
			{{0, 1, 0}, {1.0f, 1.0f, 1.0f}, {0, 1}}
		};

		static std::array<uint16_t, 6> Indices;

		bool hasChanged = false;
		
	};

}