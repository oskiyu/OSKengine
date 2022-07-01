#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IRenderpass.h"
#include "IGpuImage.h"
#include "Vector2.hpp"

#include "Sprite.h"
#include "Transform2D.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Representa una textura a la que se puede renderizar.
	/// 
	/// Incluye:
	///		- Imagen de renderizado.
	///		- Renderpass.
	///		- Sprite
	/// </summary>
	class OSKAPI_CALL RenderTarget {

	public:

		void Create(const Vector2ui& targetSize);
		void Resize(const Vector2ui& targetSize);

		GpuImage* GetTargetImage() const;
		IRenderpass* GetTargetRenderpass() const;

		const Sprite& GetSprite() const;
		const ECS::Transform2D& GetSpriteTransform() const;

	private:

		UniquePtr<GpuImage> targetImage = nullptr;
		UniquePtr<IRenderpass> targetRenderpass = nullptr;

		Sprite targetSprite;
		ECS::Transform2D spriteTransform{ ECS::EMPTY_GAME_OBJECT };

	};

}
