#include "Sprite.h"
#include <ext\matrix_transform.hpp>

namespace OSK {

	void Sprite::SetTexCoords(const Vector4& texCoords) {
		Vector4 finalTexCoords = texCoords / Vector4(texture->sizeX, texture->sizeY, texture->sizeX, texture->sizeY);

		Vertices[0].TextureCoordinates.x = finalTexCoords.X;
		Vertices[0].TextureCoordinates.y = finalTexCoords.Y;
		Vertices[1].TextureCoordinates.x = finalTexCoords.Z;
		Vertices[1].TextureCoordinates.y = finalTexCoords.Y;
		Vertices[2].TextureCoordinates.x = finalTexCoords.Z;
		Vertices[2].TextureCoordinates.y = finalTexCoords.W;
		Vertices[3].TextureCoordinates.x = finalTexCoords.X;
		Vertices[3].TextureCoordinates.y = finalTexCoords.W;
		hasChanged = true;
	}

	void Sprite::SetTexCoords(const float& x, const float& y, const float& width, const float& hegith) {
		SetTexCoords(Vector4(x, y, width, hegith));
	}

	PushConst2D Sprite::getPushConst() {
		SpriteTransform.UpdateModel();
		return PushConst2D{ SpriteTransform.ModelMatrix, color.ToGLM() };
	}

}