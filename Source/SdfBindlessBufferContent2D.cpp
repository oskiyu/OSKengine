#include "SdfBindlessBufferContent2D.h"

#include "SdfDrawCall2D.h"
#include "IGpuImageView.h"

#include "InvalidArgumentException.h"


OSK::GRAPHICS::SdfBindlessBufferContent2D OSK::GRAPHICS::SdfBindlessBufferContent2D::From(const SdfDrawCall2D& drawCall) {
	SdfBindlessBufferContent2D output{};
	output.Fill(drawCall);
	return output;
}

void OSK::GRAPHICS::SdfBindlessBufferContent2D::Fill(const SdfDrawCall2D& drawCall) {
	typeInfo.x = static_cast<int>(drawCall.shape);
	typeInfo.y = static_cast<int>(drawCall.contentType);
	typeInfo.z = static_cast<int>(drawCall.fill);

	this->matrix = drawCall.transform.GetAsMatrix();

	if (drawCall.contentType == SdfDrawCallContentType2D::TEXTURE) {
		OSK_ASSERT(
			drawCall.texture != nullptr,
			InvalidArgumentException(std::format("Si SdfDrawCall2D::contentType == SdfDrawCallContentType2D::TEXTURE, entonces SdfDrawCall2D::texture no debe ser null.")));

		textureCoords = drawCall.textureCoordinates.GetNormalized(drawCall.texture->GetSize2D().ToVector2f());
	}

	mainColor = drawCall.mainColor;
	secondaryColor = drawCall.gradientSecondColor;

	const Vector2f centerPosition = drawCall.transform.GetPosition() + drawCall.transform.GetScale() * 0.5f;

	floatInfo.x = centerPosition.x;
	floatInfo.y = centerPosition.y;
	floatInfo.Z = drawCall.borderSize;
}
