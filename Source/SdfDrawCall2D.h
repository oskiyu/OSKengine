#pragma once

#include "Vector4.hpp"
#include "Color.hpp"
#include "TextureCoordinates.h"

#include "SdfDrawCallType2D.h"
#include "SdfShape2D.h"

#include "GpuImageSamplerDesc.h"
#include "IGpuImageSampler.h"

#include "TransformComponent2D.h"


namespace OSK::GRAPHICS {

	class IGpuImageView;

	/// @brief Información necesaria para renderizar un sprite
	/// 2D usando signed-distance-fields.
	struct SdfDrawCall2D {

		/// @brief Transform del elemento 2D.
		OSK::ECS::Transform2D transform = OSK::ECS::Transform2D(ECS::GameObjectIndex::CreateEmpty());


		/// @brief Tipo de forma.
		SdfShape2D shape = SdfShape2D::RECTANGLE;

		/// @brief Tipo de relleno de la forma.
		SdfDrawCallContentType2D contentType = SdfDrawCallContentType2D::TEXTURE;


		/// @brief True si la forma está rellena.
		/// False si está hueca por dentro (sólamente contorno).
		bool fill = true;

		/// @brief Para formas huecas, grosor del contorno.
		/// Para formas rellenas, no hace nada.
		float borderSize = 1.0f;


		/// @brief Textura a usar (si shape == `SdfShape2D::RECTANGLE`).
		const IGpuImageView* texture = nullptr;

		/// @brief Coordenadas de la textura (si shape == `SdfShape2D::RECTANGLE`).
		TextureCoordinates2D textureCoordinates = TextureCoordinates2D::Normalized({ 0, 0, 1, 1 });


		/// @brief Color principal.
		/// - Para @p contentType = `SdfDrawCallContentType2D::TEXTURE`: color con el que se multiplicará.
		/// - Para @p contentType = `SdfDrawCallContentType2D::COLOR_FLAT`: color.
		/// - Para @p contentType = `SdfDrawCallContentType2D::COLOR_SIMPLE_GRADIENT`: primer color del degradado.
		Color mainColor = Color::White;

		/// @brief Color secundario.
		/// - Para @p contentType = `SdfDrawCallContentType2D::TEXTURE`: nada.
		/// - Para @p contentType = `SdfDrawCallContentType2D::COLOR_FLAT`: nada.
		/// - Para @p contentType = `SdfDrawCallContentType2D::COLOR_SIMPLE_GRADIENT`: segundo color del degradado.
		Color gradientSecondColor = Color::White;


		/// @brief Propiedades del sampler que se usará
		/// para la imagen.
		/// @pre @p contentType = `SdfDrawCallContentType2D::TEXTURE`.
		/// @pre @p sampler = `nullptr`.
		GpuImageSamplerDesc samplerDesc{};

		/// @brief Sampler que se usará para la imagen..
		/// @pre @p contentType = `SdfDrawCallContentType2D::TEXTURE`.
		const IGpuImageSampler* sampler = nullptr;

	};

}
