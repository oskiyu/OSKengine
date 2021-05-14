#include "Sprite.h"
#include <ext\matrix_transform.hpp>
#include "ToString.h"

namespace OSK {
	
	std::array<uint16_t, 6> Sprite::indices = {
			0, 1, 2, 2, 3, 0
	};

	GPUDataBuffer Sprite::indexBuffer;


	Sprite::~Sprite() {
		
	}

	void Sprite::SetTexCoordsInPercent(const Vector4f& texCoords) {
		Vector4f finalTexCoords = texCoords;
		finalTexCoords.Z += finalTexCoords.X;
		finalTexCoords.W += finalTexCoords.Y;

		vertices[0].textureCoordinates.x = finalTexCoords.X;
		vertices[0].textureCoordinates.y = finalTexCoords.Y;
		vertices[1].textureCoordinates.x = finalTexCoords.Z;
		vertices[1].textureCoordinates.y = finalTexCoords.Y;
		vertices[2].textureCoordinates.x = finalTexCoords.Z;
		vertices[2].textureCoordinates.y = finalTexCoords.W;
		vertices[3].textureCoordinates.x = finalTexCoords.X;
		vertices[3].textureCoordinates.y = finalTexCoords.W;

		hasChanged = true;
	}

	void Sprite::SetTexCoords(const Vector4f& texCoords) {
		Vector4f finalTexCoords = texCoords / Vector4f(texture->size.X, texture->size.Y, texture->size.X, texture->size.Y);

		SetTexCoordsInPercent(finalTexCoords);
	}

	void Sprite::UpdateMaterialTexture() {
		material->SetTexture("Texture", texture);
	}

	PushConst2D Sprite::getPushConst(const glm::mat4& camera) {
		transform.UpdateModel();
		return PushConst2D{ transform.AsMatrix(), camera, color.ToGLM() };
	}

}