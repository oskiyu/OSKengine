#pragma once

#include "VulkanBuffer.h"
#include "Vertex.h"
#include "Texture.h"

#include <array>

#include <glm.hpp>
#include "Transform2D.h"
#include "Color.h"

#include "Material.h"

namespace OSK {

	//Push constants de un sprite.
	struct OSKAPI_CALL PushConst2D {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 camera = glm::mat4(1.0f);
		alignas(16) glm::vec4 color = glm::vec4(1.0f);
	};

	//Representa un sprite, una textura con un transform.
	class OSKAPI_CALL Sprite {

		friend class RenderAPI;
		friend class ContentManager;
		friend class SpriteBatch;
		friend class RenderSystem3D;
		friend class SpriteContainer;

	public:

		~Sprite();

		//Establece la región de la textura que se renderizará en este sprite.
		void SetTexCoords(const Vector4& texCoords);

		void SetTexCoordsInPercent(const Vector4f& texCoords);

		//Establece la región de la textura que se renderizará en este sprite.
		void SetTexCoords(float x, float y, float width, float hegith);

		void UpdateMaterialTexture();

		//Transform del sprite.
		Transform2D SpriteTransform;

		//Textura que se renderiza en este sprite.
		Texture* Texture2D = nullptr;

		SharedPtr<MaterialInstance> SpriteMaterial;

		//Color del sprite.
		Color color = Color(1.0f);

	private:

		bool isOutOfScreen = false;

		PushConst2D getPushConst(const glm::mat4& camera);

		glm::mat4 model = glm::mat4(1);
		
		Vector4 rectangle{};

		float_t rotation = 0.0f;

		VulkanBuffer VertexBuffer;

		static VulkanBuffer IndexBuffer;

		bool hasChanged = false;

	public:

		std::vector<Vertex> Vertices = {
			{{0, 0, 0}, {1.0f, 1.0f, 1.0f}, {0, 0}},
			{{1, 0, 0}, {1.0f, 1.0f, 1.0f}, {1, 0}},
			{{1, 1, 0}, {1.0f, 1.0f, 1.0f}, {1, 1}},
			{{0, 1, 0}, {1.0f, 1.0f, 1.0f}, {0, 1}}
		};

		static std::array<uint16_t, 6> Indices;
				
	};

}