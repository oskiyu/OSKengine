#pragma once

#include "GPUDataBuffer.h"
#include "Vertex.h"
#include "Texture.h"

#include <array>

#include <glm.hpp>
#include "Transform2D.h"
#include "Color.h"

#include "Material.h"
#include "PushConst2D.h"

namespace OSK {


	/// <summary>
	/// Representa un sprite: una textura que puede ser renderizada.
	/// Contiene su posición, rotación, color...
	/// </summary>
	class OSKAPI_CALL Sprite {

		friend class RenderAPI;
		friend class ContentManager;
		friend class SpriteBatch;
		friend class RenderSystem3D;
		friend class SpriteContainer;

	public:

		/// <summary>
		/// Elimina el sprite.
		/// </summary>
		~Sprite();

		/// <summary>
		/// Establece la región (en píxeles) de la textura que se renderizará en este sprite.
		/// </summary>
		/// <param name="texCoords">Coordenadas (en píxeles).</param>
		void SetTexCoords(const Vector4& texCoords);

		/// <summary>
		/// Establece la región (en porcentaje sobre el tamaño total) de la textura que se renderizará en este sprite.
		/// </summary>
		/// <param name="texCoords">Coordenadas porcentuales.</param>
		void SetTexCoordsInPercent(const Vector4f& texCoords);

		/// <summary>
		/// Transform del sprite.
		/// </summary>
		Transform2D transform;

		/// <summary>
		/// Material del sprite.
		/// </summary>
		SharedPtr<MaterialInstance> material;

		/// <summary>
		/// Color del sprite.
		/// </summary>
		Color color = Color(1.0f);

		void SetTexture(Texture* texture);
		Texture* GetTexture() const;

	private:

		/// <summary>
		/// Textura que se renderiza en este sprite.
		/// </summary>
		Texture* texture = nullptr;

		/// <summary>
		/// Obtiene e push const 2D del sprite.
		/// </summary>
		/// <param name="camera">Matriz de la cámara usada para renderizarlo.</param>
		/// <returns>Push const 2D.</returns>
		PushConst2D getPushConst();

		/// <summary>
		/// Matriz modelo del sprite.
		/// </summary>
		glm::mat4 model = glm::mat4(1);
		
		/// <summary>
		/// Área del sprite.
		/// </summary>
		Vector4 textureCoords{};

		/// <summary>
		/// Rotación del sprite, en ángulo hacia lad erecha.
		/// </summary>
		float_t rotation = 0.0f;

		/// <summary>
		/// Vértices del sprite.
		/// </summary>
		static SharedPtr<GpuDataBuffer> vertexBuffer;

		/// <summary>
		/// Índices del sprite.
		/// </summary>
		static SharedPtr<GpuDataBuffer> indexBuffer;

		/// <summary>
		/// Vértices.
		/// </summary>
		static std::array<Vertex2D, 4> vertices;

		/// <summary>
		/// Índices.
		/// </summary>
		static std::array<uint16_t, 6> indices;
				
	};

}
