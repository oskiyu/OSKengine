#pragma once

#include "OSKmacros.h"
#include "Color.hpp"
#include "Vector4.hpp"

namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::GRAPHICS {

	class GpuImage;

	class IGpuVertexBuffer;
	class IGpuIndexBuffer;

	/// <summary>
	/// Representa un sprite: una textura que puede ser renderizada.
	/// </summary>
	class OSKAPI_CALL Sprite {

	public:

		/// <summary>
		/// Establece la región (en píxeles) de la textura que se renderizará en este sprite.
		/// </summary>
		/// <param name="texCoords">Coordenadas (en píxeles).</param>
		void SetTexCoords(const Vector4f& texCoords);

		/// <summary>
		/// Establece la región (en porcentaje sobre el tamaño total) de la textura que se renderizará en este sprite.
		/// </summary>
		/// <param name="texCoords">Coordenadas porcentuales.</param>
		void SetTexCoordsInPercent(const Vector4f& texCoords);

		/// <summary>
		/// Devuelve las coordenadas de textura del sprite,
		/// normalizadas (0.0 - 1.0).
		/// </summary>
		Vector4f GetTexCoords() const;

		/// <summary>
		/// Establece la textura del sprite.
		/// </summary>
		void SetTexture(const ASSETS::Texture* texture);

		/// <summary>
		/// Establece la textura del sprite.
		/// </summary>
		void SetGpuImage(const GpuImage* image);

		/// <summary>
		/// Devuelve la imagen que es renderizada.
		/// </summary>
		const GpuImage* GetGpuImage() const;

		/// <summary>
		/// Color del sprite.
		/// Si es blanco, se renderiza la imagen tal cual.
		/// Si es de otro color, tendrá un tinte.
		/// Puede usarse para modificar su transparencia.
		/// </summary>
		Color color = Color(1.0f);

		/// <summary>
		/// Todos los spirtes comparten los mismos vértices.
		/// </summary>
		static IGpuVertexBuffer* globalVertexBuffer;

		/// <summary>
		/// Todos los sprites comparten los mismos índices.
		/// </summary>
		static IGpuIndexBuffer* globalIndexBuffer;

	private:

		Vector4f texCoords = { 0, 0, 1, 1 };
		const GpuImage* image = nullptr;

	};

}
