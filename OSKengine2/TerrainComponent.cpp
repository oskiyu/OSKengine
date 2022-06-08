#include "TerrainComponent.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Color.hpp"
#include "Vertex3D.h"

using namespace OSK;
using namespace OSK::GRAPHICS;
using namespace OSK::ECS;

void TerrainComponent::Generate(const Vector2ui& resolution) {
	DynamicArray<Vertex3D> terrainVertices;
	DynamicArray<TIndexSize> terrainIndices;

	for (TSize posX = 0; posX < resolution.X; posX++) {
		for (TSize posY = 0; posY < resolution.Y; posY++) {
			terrainVertices.Insert(Vertex3D{
				(Vector3f(posX, 0, posY) / Vector3f(resolution.X, 1, resolution.Y)),
				 Vector3f(0),
				 Color::WHITE(),
				 Vector2f(posX, posY) / resolution.ToVector2f()
				});
		}
	}

	for (TSize y = 0; y < resolution.Y - 1; y++) {
		for (TSize x = 0; x < resolution.X - 1; x++) {
			terrainIndices.InsertAll({ x + y * resolution.X, x + 1 + y * resolution.X, x + (y + 1) * resolution.X,
									   x + 1 + (y + 1) * resolution.X, x + 1 + y * resolution.X, x + (y + 1) * resolution.X });
		}
	}

	vertexBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(terrainVertices).GetPointer();
	indexBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(terrainIndices).GetPointer();

	numIndices = terrainIndices.GetSize();
}

void TerrainComponent::SetMaterialInstance(OwnedPtr<MaterialInstance> materialInstance) {
	this->materialInstance = materialInstance.GetPointer();
}

const IGpuVertexBuffer* TerrainComponent::GetVertexBuffer() const {
	return vertexBuffer.GetPointer();
}

const IGpuIndexBuffer* TerrainComponent::GetIndexBuffer() const {
	return indexBuffer.GetPointer();
}


MaterialInstance* TerrainComponent::GetMaterialInstance() const {
	return materialInstance.GetPointer();
 }

TSize TerrainComponent::GetNumIndices() const {
	return numIndices;
}
