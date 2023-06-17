#include "TextureCoordinates.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

Vector4f TextureCoordinates2D::GetNormalized(const Vector2f& imageSize) const {
	if (type == TextureCoordsType::NORMALIZED)
		return texCoords;
	else
		return texCoords / Vector4f(imageSize.x, imageSize.y, imageSize.x, imageSize.y);
}
