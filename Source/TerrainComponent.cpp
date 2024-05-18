#include "TerrainComponent.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Color.hpp"
#include "Vertex3D.h"
#include "MathExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;
using namespace OSK::ECS;

void TerrainComponent::Generate(const Vector2ui& resolution) {
	DynamicArray<Vertex3D> terrainVertices;
	DynamicArray<TIndexSize> terrainIndices;

	for (UIndex32 posX = 0; posX < resolution.x; posX++) {
		for (UIndex32 posY = 0; posY < resolution.y; posY++) {
			terrainVertices.Insert(Vertex3D{
				(Vector3f(static_cast<float>(posX), 0, static_cast<float>(posY)) / Vector3f((float)resolution.x, 1, (float)resolution.y)),
				 Vector3f(0),
				 Color::White,
				 Vector2f(static_cast<float>(posX), static_cast<float>(posY)) / resolution.ToVector2f()
				});
		}
	}

	for (UIndex32 y = 0; y < resolution.y - 1; y++) {
		for (UIndex32 x = 0; x < resolution.x - 1; x++) {
			terrainIndices.InsertAll({ x + y * resolution.x, x + 1 + y * resolution.x, x + (y + 1) * resolution.x,
									   x + 1 + (y + 1) * resolution.x, x + 1 + y * resolution.x, x + (y + 1) * resolution.x });
		}
	}

	vertexBuffer = Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(terrainVertices, Vertex3D::GetVertexInfo(), GpuQueueType::MAIN).GetPointer();
	indexBuffer = Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(terrainIndices, GpuQueueType::MAIN).GetPointer();

	OSK_ASSERT(
		terrainIndices.GetSize() < std::numeric_limits<USize32>::max(), 
		OverflowConversionException()
	);

	numIndices = static_cast<USize32>(terrainIndices.GetSize());
}

void TerrainComponent::SetMaterialInstance(OwnedPtr<MaterialInstance> materialInstance) {
	this->materialInstance = materialInstance.GetPointer();
}

const GpuBuffer* TerrainComponent::GetVertexBuffer() const {
	return vertexBuffer.GetPointer();
}

const GpuBuffer* TerrainComponent::GetIndexBuffer() const {
	return indexBuffer.GetPointer();
}


MaterialInstance* TerrainComponent::GetMaterialInstance() const {
	return materialInstance.GetPointer();
 }

USize32 TerrainComponent::GetNumIndices() const {
	return numIndices;
}

template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::TerrainComponent>(const OSK::ECS::TerrainComponent& data) {
	nlohmann::json output{};

	return output;
}

template <>
OSK::ECS::TerrainComponent PERSISTENCE::DeserializeComponent<OSK::ECS::TerrainComponent>(const nlohmann::json& json, const SavedGameObjectTranslator&) {
	return {};
}

template <>
PERSISTENCE::BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::TerrainComponent>(const OSK::ECS::TerrainComponent& data) {
	return BinaryBlock::Empty();
}

template <>
OSK::ECS::TerrainComponent PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::TerrainComponent>(PERSISTENCE::BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	return {};
}
