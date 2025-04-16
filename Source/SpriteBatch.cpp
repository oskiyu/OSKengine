#include "SpriteBatch.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "IGpu.h"

#include "Vertex2D.h"
#include "ICommandList.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

SpriteBatch::SpriteBatch(UniquePtr<MaterialInstance>&& materialInstance) : materialInstance(std::move(materialInstance)) {
	auto& allocator = *Engine::GetRenderer()->GetAllocator();

	const USize64 vertexBufferSize = sizeof(Vertex2D) * maxVertices;
	const USize64 indexBufferSize = sizeof(TIndexSize) * maxIndices;

	for (auto& buffer : vertexBuffers) {
		buffer = allocator.CreateBuffer(vertexBufferSize, GPU_MEMORY_NO_ALIGNMENT, GpuBufferUsage::VERTEX_BUFFER, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN);
		buffer->MapMemory();
	}

	for (auto& buffer : indexBuffers) {
		buffer = allocator.CreateBuffer(indexBufferSize, GPU_MEMORY_NO_ALIGNMENT, GpuBufferUsage::INDEX_BUFFER, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN);
		buffer->MapMemory();
	}
}

SpriteBatch::~SpriteBatch() {
	for (auto& buffer : vertexBuffers)
		buffer->Unmap();

	for (auto& buffer : indexBuffers)
		buffer->Unmap();
}


void SpriteBatch::Draw(Vector2f position, Vector2f size, Vector4f texCoords) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const Vertex2D newVertices[4] {
		Vertex2D(position + size * Vector2f(0, 0), { texCoords.x, texCoords.y }),
		Vertex2D(position + size * Vector2f(1, 0), { texCoords.z, texCoords.y }),
		Vertex2D(position + size * Vector2f(1, 1), { texCoords.z, texCoords.w }),
		Vertex2D(position + size * Vector2f(0, 1), { texCoords.x, texCoords.w })
	};

	const TIndexSize newIndices[6] {
		0 + numIndices, 1 + numIndices, 2 + numIndices,
		1 + numIndices, 2 + numIndices, 3 + numIndices
	};

	vertexBuffers[resourceIndex]->WriteOffset(newVertices, 4 * sizeof(Vertex2D), numVertices * sizeof(Vertex2D));
	indexBuffers[resourceIndex]->WriteOffset(newIndices, 6 * sizeof(TIndexSize), numIndices * sizeof(TIndexSize));

	numVertices += 4;
	numIndices += 6;
}

void SpriteBatch::ResetCurrentFrame() {
	numVertices = 0;
	numIndices = 0;
}

void SpriteBatch::Draw(ICommandList* commandList) {
	if (numIndices == 0)
		return;

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	VertexBufferView vertexView{};
	vertexView.offsetInBytes = 0;
	vertexView.vertexInfo = Vertex2D::GetVertexInfo();
	vertexView.numVertices = numVertices;

	IndexBufferView indexView{};
	indexView.offsetInBytes = 0;
	indexView.type = IndexType::U32;
	indexView.numIndices = numIndices;

	commandList->BindVertexBufferRange(vertexBuffers[resourceIndex].GetValue(), vertexView);
	commandList->BindIndexBufferRange(indexBuffers[resourceIndex].GetValue(), indexView);
	commandList->DrawSingleInstance(numIndices);
}
