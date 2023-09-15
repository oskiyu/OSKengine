#pragma once

#include "OSKmacros.h"
#include "Color.hpp"
#include "Vector4.hpp"
#include "Component.h"
#include "MaterialInstance.h"
#include "UniquePtr.hpp"
#include "TextureCoordinates.h"
#include "OwnedPtr.h"

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
	/// Tambi�n es un componente para objetos 2D.
	class OSKAPI_CALL Sprite {

	public:

		Sprite();

		OSK_COMPONENT("OSK::Sprite");

	public:

		/// @brief Establece el material sobre el que se inicializar�
		/// el slot de la imagen del sprite.
		/// @param material Material 2D.
		/// 
		/// @pre El layout de @p material debe tener registrado un slot llamado "textura".
		/// @throws InvalidMaterialException Si no se cumple la precondici�n.
		void SetupMaterial(Material* material);

		
		/// @brief Establece la regi�n de la imagen que se renderizar� en este sprite.
		/// @param texCoords Regi�n de la imagen.
		void SetTexCoords(const TextureCoordinates2D& texCoords);

		/// @return Devuelve las coordenadas de la imagen del sprite.
		TextureCoordinates2D GetTexCoords() const;

		
		/// @brief Establece la imagen del sprite.
		/// @param view View de la imagen que se renderizar�.
		/// Si es null, no renderizar� nada.
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

		/// @brief Buffer de v�rtices global para el renderizado en modo inmediato.
		static GpuBuffer* globalVertexBuffer;
		/// @brief Buffer de �ndices global para el renderizado en modo inmediato.
		static GpuBuffer* globalIndexBuffer;

	private:

		TextureCoordinates2D texCoords = TextureCoordinates2D::Normalized({ 0, 0, 1, 1 });
		const IGpuImageView* view = nullptr;
		UniquePtr<MaterialInstance> textureMaterialInstance;

	};

}
