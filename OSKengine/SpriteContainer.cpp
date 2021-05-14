#include "SpriteContainer.h"

using namespace OSK;

void SpriteContainer::Set(Sprite& sprite, glm::mat4 cameraMat) {
	spriteMaterial = sprite.material.GetPointer();
	color = sprite.color;
	pushConst = sprite.getPushConst(cameraMat);
	vertexBuffer = sprite.vertexBuffer.buffer;
	vertexMemory = sprite.vertexBuffer.memory;
	hasChanged = sprite.hasChanged;

	for (uint32_t i = 0; i < 4; i++)
		vertices[i] = sprite.vertices[i];
}