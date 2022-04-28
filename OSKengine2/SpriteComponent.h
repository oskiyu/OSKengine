#pragma once

#include "Component.h"
#include "Sprite.h"
#include "UniquePtr.hpp"
#include "MaterialInstance.h"

namespace OSK::GRAPHICS {
	class GpuImage;
}
namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::ECS {

	class CameraComponent2D;

	/// <summary>
	/// Componente que permite a un GameObject tener
	/// un sprite asociado.
	/// 
	/// @pre El objeto debe tener también un Transform2D.
	/// 
	/// @note Se encarga de manejar el material 2D por defecto.
	/// </summary>
	class OSKAPI_CALL SpriteComponent {

	public:

		OSK_COMPONENT("OSK::SpriteComponent");

		GRAPHICS::Sprite& GetSprite();
		const GRAPHICS::Sprite& GetSprite() const;

		/// <summary>
		/// Debe establecerse la cámara después de instanciar
		/// la instancia del material 2D.
		/// </summary>
		/// 
		/// @warning Debe ser establecido para poder ser usado.
		void SetCamera(const CameraComponent2D& camera);

		/// <summary>
		/// Establece la instancia del material 2D.
		/// </summary>
		/// 
		/// @warning Debe ser establecido para poder ser usado.
		void SetMaterialInstance(GRAPHICS::MaterialInstance* mInstance);
		GRAPHICS::MaterialInstance* GetMaterialInstance() const;

		/// <summary>
		/// Debe establecerse la textura después de instanciar
		/// la instancia del material 2D.
		/// </summary>
		/// 
		/// @warning Debe ser establecido para poder ser usado.
		void SetTexture(const ASSETS::Texture* texture);

		/// <summary>
		/// Debe establecerse la textura después de instanciar
		/// la instancia del material 2D.
		/// </summary>
		/// 
		/// @warning Debe ser establecido para poder ser usado.
		void SetGpuImage(const GRAPHICS::GpuImage* image);

	private:

		GRAPHICS::Sprite sprite{};
		UniquePtr<GRAPHICS::MaterialInstance> materialInstance;

	};

}
