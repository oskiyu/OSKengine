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

		explicit ImageView(const Vector2f& size) : IElement(size) {}

	};

}
