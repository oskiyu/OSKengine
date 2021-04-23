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
	/// Contiene su posici�n, rotaci�n, color...
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
		/// Establece la regi�n (en p�xeles) de la textura que se renderizar� en este sprite.
		/// </summary>
		/// <param name="texCoords">Coordenadas (en p�xeles).</param>
		void SetTexCoords(const Vector4& texCoords);

		/// <summary>
		/// Establece la regi�n (en porcentaje sobre el tama�o total) de la textura que se renderizar� en este sprite.
		/// </summary>
		/// <param name="texCoords">Coordenadas porcentuales.</param>
		void SetTexCoordsInPercent(const Vector4f& texCoords);

		/// <summary>
		/// Actualiza el material del sprite, para que use la textura de este sprite.
		/// Puede usarse para actualizar la textura.
		/// </summary>
		void UpdateMaterialTexture();

		/// <summary>
		/// Transform del sprite.
		/// </summary>
		Transform2D SpriteTransform;

		/// <summary>
		/// Textura que se renderiza en este sprite.
		/// </summary>
		Texture* Texture2D = nullptr;

		/// <summary>
		/// Material del sprite.
		/// </summary>
		SharedPtr<MaterialInstance> SpriteMaterial;

		/// <summary>
		/// Color del sprite.
		/// </summary>
		Color color = Color(1.0f);

	private:

		/// <summary>
		/// True si est� completamente fuera de la pantalla (no se ve).
		/// Establecido por el renderizador.
		/// </summary>
		bool isOutOfScreen = false;

		/// <summary>
		/// Obtiene e push const 2D del sprite.
		/// </summary>
		/// <param name="camera">Matriz de la c�mara usada para renderizarlo.</param>
		/// <returns>Push const 2D.</returns>
		PushConst2D getPushConst(const glm::mat4& camera);

		/// <summary>
		/// Matriz modelo del sprite.
		/// </summary>
		glm::mat4 model = glm::mat4(1);
		
		/// <summary>
		/// �rea del sprite.
		/// </summary>
		Vector4 rectangle{};

		/// <summary>
		/// Rotaci�n del sprite, en �ngulo hacia lad erecha.
		/// </summary>
		float_t rotation = 0.0f;

		/// <summary>
		/// V�rtices del sprite.
		/// </summary>
		GPUDataBuffer VertexBuffer;

		/// <summary>
		/// �ndices del sprite.
		/// </summary>
		static GPUDataBuffer IndexBuffer;

		/// <summary>
		/// True si hay que actualizar el buffer de v�rtices (por cambio de coordenadas de textura).
		/// </summary>
		bool hasChanged = false;

	public:

		/// <summary>
		/// V�rtices.
		/// </summary>
		std::vector<Vertex> Vertices = {
			{{0, 0, 0}, {1.0f, 1.0f, 1.0f}, {0, 0}},
			{{1, 0, 0}, {1.0f, 1.0f, 1.0f}, {1, 0}},
			{{1, 1, 0}, {1.0f, 1.0f, 1.0f}, {1, 1}},
			{{0, 1, 0}, {1.0f, 1.0f, 1.0f}, {0, 1}}
		};

		/// <summary>
		/// �ndices.
		/// </summary>
		static std::array<uint16_t, 6> Indices;
				
	};

}