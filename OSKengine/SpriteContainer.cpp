#include "SpriteContainer.h"

using namespace OSK;

void SpriteContainer::Set(Sprite& sprite, glm::mat4 cameraMat) {
	SpriteMaterial = sprite.SpriteMaterial.GetPointer();
	SpriteColor = sprite.color;
	PConst = sprite.getPushConst(cameraMat);
	VertexBuffer = sprite.VertexBuffer.Buffer;
	VertexMemory = sprite.VertexBuffer.Memory;
	hasChanged = sprite.hasChanged;

	for (uint32_t i = 0; i < 4; i++)
		Vertices[i] = sprite.Vertices[i];
}