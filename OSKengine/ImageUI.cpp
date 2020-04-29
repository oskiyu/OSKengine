#include "ImageUI.h"

namespace OSK::UI {

	Image::Image(const Vector4& rectangle) : BaseUIElement(rectangle) {

	}

	Image::~Image() {

	}

	void Image::Draw(const RenderAPI& renderer) {
		IUserInterfaceImage::Draw(renderer, *this);
	}

}