#pragma once

#include "UiElement.h"
#include "IGpuImageView.h"
#include "Sprite.h"

#include <array>
#include <ranges>

namespace OSK::GRAPHICS {
	class IGpuImageView;
}

namespace OSK::UI {

	/// @brief Elemento de UI que muestra una imagen.
	class OSKAPI_CALL ImageView : public IElement {

	public:

		ImageView(const Vector2f& size) : IElement(size) {}

	public:

		/// @return Referencia constante al sprite rendereizado.
		inline const GRAPHICS::Sprite& GetSprite() const { return sprite; }

		/// @return Referencia al sprite renderizado.
		inline GRAPHICS::Sprite& GetSprite() { return sprite; }

		void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition) const override;

	private:

		GRAPHICS::Sprite sprite;

	};

}
