#include "ImageUI.h"

namespace OSK::UI {

	Image::Image(const Vector4& rectangle) : BaseUIElement(rectangle) {
		Rectangle = &this->rectangle;
	}

	Image::~Image() {

	}

	void Image::Draw(SpriteBatch& spriteBatch) {
		IUserInterfaceImage::Draw(spriteBatch, *this);
	}

}