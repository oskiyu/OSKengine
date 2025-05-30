#pragma once

#include "ApiCall.h"
#include "Color.hpp"
#include "Vector4.hpp"
#include "Component.h"
#include "MaterialInstance.h"
#include "UniquePtr.hpp"
#include "TextureCoordinates.h"

#include "AssetRef.h"
#include "Texture.h"

namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::ECS {
	class CameraComponent2D;
}

namespace OSK::GRAPHICS {

	class GpuImage;

	
	/// @brief Un sprite representa una imagen que puede ser renderizada en 2D.
	/// 
	/// También es un componente para objetos 2D.
	/// 
	/// @todo Serialización.
	class OSKAPI_CALL Sprite {

	public:

		OSK_SERIALIZABLE_COMPONENT();

	public:

		Sprite();

		OSK_COMPONENT("OSK::Sprite");

	public:

		/// @brief Establece el material sobre el que se inicializará
		/// el slot de la imagen del sprite.
		/// @param material Material 2D.
		/// 
		/// @pre El layout de @p material debe tener registrado un slot llamado "textura".
		/// @throws InvalidMaterialException Si no se cumple la precondición.
		void SetupMaterial(Material* material);

		
		/// @brief Establece la región de la imagen que se renderizará en este sprite.
		/// @param texCoords Región de la imagen.
		void SetTexCoords(const TextureCoordinates2D& texCoords);

		/// @return Devuelve las coordenadas de la imagen del sprite.
		TextureCoordinates2D GetTexCoords() const;

		
		void LinkAsset(ASSETS::AssetRef<ASSETS::Texture> texture);

		/// @brief Establece la imagen del sprite.
		/// @param view View de la imagen que se renderizará.
		/// Si es null, no renderizará nada.
		/// 
		/// @pre @p view debe tener estabilidad de puntero.
		void SetImageView(const IGpuImageView* view);

		/// @brief Color de la imagen (incluyendo transparencia).
		/// Por defecto, blanco.
		Color color = Color::White;


		/// @return Slot "texture", que contiene un sampler con la imagen
		/// del sprite.
		const IMaterialSlot* GetTextureSlot() const;

		/// @return View de la imagen del sprite.
		const IGpuImageView* GetView() const;

		/// @brief Buffer de vértices global para el renderizado en modo inmediato.
		static UniquePtr<GpuBuffer> globalVertexBuffer;
		/// @brief Buffer de índices global para el renderizado en modo inmediato.
		static UniquePtr<GpuBuffer> globalIndexBuffer;

	private:

		TextureCoordinates2D texCoords = TextureCoordinates2D::Normalized({ 0, 0, 1, 1 });
		ASSETS::AssetRef<ASSETS::Texture> m_texture;
		const IGpuImageView* view = nullptr;
		UniquePtr<MaterialInstance> textureMaterialInstance;

	};

}

OSK_COMPONENT_SERIALIZATION(OSK::GRAPHICS::Sprite);
