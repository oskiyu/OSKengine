#include "Sprite.h"
#include <ext\matrix_transform.hpp>
#include "ToString.h"

namespace OSK {
	
	std::array<uint16_t, 6> Sprite::Indices = {
			0, 1, 2, 2, 3, 0
	};

	GPUDataBuffer Sprite::IndexBuffer;


	Sprite::~Sprite() {
		
	}

	void Sprite::SetTexCoordsInPercent(const Vector4f& texCoords) {
		Vector4f finalTexCoords = texCoords;
		finalTexCoords.Z += finalTexCoords.X;
		finalTexCoords.W += finalTexCoords.Y;

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

	void Sprite::SetTexCoords(const Vector4f& texCoords) {
		Vector4f finalTexCoords = texCoords / Vector4f(Texture2D->Size.X, Texture2D->Size.Y, Texture2D->Size.X, Texture2D->Size.Y);

		SetTexCoordsInPercent(finalTexCoords);
	}

	void Sprite::UpdateMaterialTexture() {
		SpriteMaterial->SetTexture("Texture", Texture2D);
	}

	PushConst2D Sprite::getPushConst(const glm::mat4& camera) {
		SpriteTransform.UpdateModel();
		return PushConst2D{ SpriteTransform.ModelMatrix, camera, color.ToGLM() };
	}

}