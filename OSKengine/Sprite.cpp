#include "Sprite.h"
#include <ext\matrix_transform.hpp>
#include "ToString.h"

#include "MaterialSlot.h"

namespace OSK {

	std::array<Vertex2D, 4> Sprite::vertices = {
		Vertex2D{{0, 0}},
		Vertex2D{{1, 0}},
		Vertex2D{{1, 1}},
		Vertex2D{{0, 1}}
	};

	std::array<uint16_t, 6> Sprite::indices = {
			0, 1, 2, 2, 3, 0
	};

	SharedPtr<GpuDataBuffer> Sprite::vertexBuffer;
	SharedPtr<GpuDataBuffer> Sprite::indexBuffer;


	Sprite::~Sprite() {
		
	}

	void Sprite::SetTexCoordsInPercent(const Vector4f& texCoords) {
		this->textureCoords = texCoords;
		this->textureCoords.Z += texCoords.X;
		this->textureCoords.W += texCoords.Y;

		this->textureCoords.Z -= this->textureCoords.X;
		this->textureCoords.W -= this->textureCoords.Y;
	}

	void Sprite::SetTexCoords(const Vector4f& texCoords) {
		Vector4f finalTexCoords = texCoords / Vector4ui(texture->size.X, texture->size.Y, texture->size.X, texture->size.Y).ToVector4f();

		SetTexCoordsInPercent(finalTexCoords);
	}

	void Sprite::SetTexture(Texture* texture) {
		this->texture = texture;

		material->GetMaterialSlot(MSLOT_TEXTURE_2D)->SetTexture("Texture", texture);
		material->GetMaterialSlot(MSLOT_TEXTURE_2D)->FlushUpdate();
	}

	PushConst2D Sprite::getPushConst() {
		transform.UpdateModel();

		return PushConst2D{ transform.AsMatrix(), color.ToGLM(), textureCoords.ToGLM() };
	}

	Texture* Sprite::GetTexture() const {
		return texture;
	}

}
