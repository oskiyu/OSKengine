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

	class IGpuVertexBuffer;
	class IGpuIndexBuffer;

	/// <summary>
	/// Representa un sprite: una textura que puede ser renderizada.
	/// También es un componente que permite a un GameObject tener
	/// un sprite asociado.
	/// 
	/// @note Se encarga de manejar el material 2D por defecto.
	/// 
	/// @warning Antes de ser usado para renderizado, se deben establecer
	/// la instancia del material, la cámara y la imagen.
	/// </summary>
	class OSKAPI_CALL Sprite {

	public:

		OSK_COMPONENT("OSK::Sprite");

		/// <summary>
		/// Establece la región de la textura que se renderizará en este sprite.
		/// </summary>
		void SetTexCoords(const TextureCoordinates2D& texCoords);

		/// <summary>
		/// Devuelve las coordenadas de textura del sprite,
		/// normalizadas (0.0 - 1.0).
		/// </summary>
		TextureCoordinates2D GetTexCoords() const;

		/// <summary>
		/// Debe establecerse la cámara después de instanciar
		/// la instancia del material 2D.
		/// </summary>
		/// 
		/// @pre Se debe haber establecido la instancia del
		/// material (Sprite::SetMaterialInstance).
		void SetCamera(const ECS::CameraComponent2D& camera);

		/// <summary>
		/// Establece la textura del sprite.
		/// </summary>
		/// 
		/// @pre Se debe haber establecido la instancia del
		/// material (Sprite::SetMaterialInstance).
		void SetTexture(const ASSETS::Texture* texture);

		/// <summary>
		/// Establece la textura del sprite.
		/// </summary>
		/// 
		/// @pre Se debe haber establecido la instancia del
		/// material (Sprite::SetMaterialInstance).
		void SetGpuImage(const GpuImage* image);

		/// <summary>
		/// Devuelve la imagen que es renderizada.
		/// </summary>
		const GpuImage* GetGpuImage() const;

		/// <summary>
		/// Color del sprite.
		/// 
		/// @note Si es blanco, se renderiza la imagen tal cual.
		/// @note Si es de otro color, tendrá un tinte.
		/// @note Puede usarse para modificar su transparencia.
		/// </summary>
		Color color = Color(1.0f);

		/// <summary>
		/// Establece la instancia del material que pertenecerá a este
		/// sprite.
		/// </summary>
		/// 
		/// @pre La instancia debe corresponder al material 2D por
		/// defecto (o compatible).
		void SetMaterialInstance(OwnedPtr<MaterialInstance> instance);

		/// <summary>
		/// Devuelve la instancia del material del sprite.
		/// Null si no se ha establecido.
		/// </summary>
		MaterialInstance* GetMaterialInstance() const;

		/// <summary>
		/// @note Todos los spirtes comparten los mismos vértices.
		/// </summary>
		static IGpuVertexBuffer* globalVertexBuffer;

		/// <summary>
		/// @note Todos los sprites comparten los mismos índices.
		/// </summary>
		static IGpuIndexBuffer* globalIndexBuffer;

	private:

		TextureCoordinates2D texCoords = TextureCoordinates2D::Normalized({ 0, 0, 1, 1 });
		const GpuImage* image = nullptr;
		UniquePtr<MaterialInstance> materialInstance;

	};

}
