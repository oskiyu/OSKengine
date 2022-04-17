#include "Sprite.h"

#include "IGpuImage.h"
#include "Assert.h"
#include "Texture.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IGpuVertexBuffer* Sprite::globalVertexBuffer = nullptr;
IGpuIndexBuffer* Sprite::globalIndexBuffer = nullptr;

void Sprite::SetTexCoords(const Vector4f& texCoords) {
	OSK_ASSERT(image != nullptr, "No se ha establecido la imagen del sprite.");
	SetTexCoordsInPercent({
		texCoords.GetRectanglePosition().X / image->GetSize().X,
		texCoords.GetRectanglePosition().Y / image->GetSize().Y,
		texCoords.GetRectangleSize().X / image->GetSize().X,
		texCoords.GetRectangleSize().Y / image->GetSize().Y
		});
}

void Sprite::SetTexCoordsInPercent(const Vector4f& texCoords) {
	this->texCoords = texCoords;
}

Vector4f Sprite::GetTexCoords() const {
	return texCoords;
}

void Sprite::SetTexture(const Texture* texture) {
	SetGpuImage(texture->GetGpuImage());
}

void Sprite::SetGpuImage(const GpuImage* image) {
	this->image = image;
}

const GpuImage* Sprite::GetGpuImage() const {
	return image;
}
