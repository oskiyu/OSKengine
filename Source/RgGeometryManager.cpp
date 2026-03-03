#include "RgGeometryManager.h"

#include "IRenderer.h"
#include "RgBufferManager.h"

#include "OSKengine.h"
#include "Logger.h"


using namespace OSK;
using namespace OSK::GRAPHICS;

RgGeometryManager::RgGeometryManager(
	IRenderer* renderer,
	RgBufferManager* manager,
	USize64 maxVertices)
:
	m_renderer(renderer),
	m_bufferManager(manager),
	m_maxVertices(maxVertices)
{

}

void RgGeometryManager::RegisterVertexAttribute(
	VertexAttribUuid uuid,
	std::string name,
	USize32 attributeSize,
	GdrBufferUuid bufferUuid)
{
	Engine::GetLogger()->DebugLog(std::format("Registrado atributo: {}", name));

	m_attributesInfo[uuid] = VertexAttributeEntry{
		.name = std::move(name),
		.sizeOfAttrib = attributeSize
	};
	m_attributesInfo[uuid].bufferRef = bufferUuid;

	m_attributesBufferUuids.Insert(bufferUuid);
}

GpuGeometryEntry RgGeometryManager::RegisterGeometry(const GpuGeometryDesc& geometry) {
	// Índices de la geometría.
	const auto& indices = geometry.GetIndexes();

	const auto indicesEntry = RegisterGeometryIndexes(indices.GetFullSpan());

	// Ponemos todos los índices de atributos a 0.
	// Estos índices se guardan una vez por geometría,
	// para saber en qué posiciones obtener los atributos
	// de cada uno de los vértices.
	auto attributesIndices = DynamicArray<USize32>::CreateResized(m_attributesInfo.size(), 0);

	// Procesamos cada uno de los atributos,
	// añadiéndolos a los buffers correspondientes.
	USize32 numVertices = 0;
	// Contiene las entradas para cada uno de los atributos.
	std::unordered_map<VertexAttribUuid, GlobalVertexBufferRange> vertexEntriesPerAttrib{};
	for (const auto& [uuid, attribs] : geometry.GetAttributesMap().attributes) {

		// Registramos los atributos.
		const auto entry = m_verticesBufferManager[uuid].RegisterGeometry(attribs.vertexCount);
		vertexEntriesPerAttrib[uuid] = entry;
		const USize64 size   = attribs.structSize * entry.vertexCount;
		const USize64 offset = attribs.structSize * entry.firstVertex;

		// Actualizamos el número máximo de vértices (puede que no todos
		// los vértices tengan los mismos atributos).
		numVertices = glm::max(numVertices, attribs.vertexCount);

		// Escribimos los valores en GPU.
		auto cmdList = m_renderer->CreateSingleUseCommandList(GpuQueueType::MAIN);
		cmdList->Start();

		auto stagingBuffer = m_renderer->GetAllocator()->CreateStagingBuffer(
			size,
			GpuQueueType::MAIN);

		stagingBuffer->MapMemory();
		stagingBuffer->Write(attribs.listStart, size - attribs.structSize);
		stagingBuffer->Unmap();

		cmdList->CopyBufferToBuffer(
			stagingBuffer.GetValue(),
			&m_bufferManager->GetBuffer(m_attributesInfo[uuid].bufferRef),
			size,
			0,			// <- offset en source
			offset);	// <- offset en destino

		cmdList->RegisterStagingBuffer(std::move(stagingBuffer));

		cmdList->Close();

		m_renderer->SubmitSingleUseCommandList(std::move(cmdList));

		// Índice del primer vértice dentro del buffer
		// de este atributo en concreto.
		attributesIndices[m_attributesInfo[uuid].indexInsideTable] = entry.firstVertex;
	}

	// Escribimos los índices de los atributos de la geometría.
	const auto entry = m_attributesIndicesBufferManager.RegisterGeometry(numVertices);
	{
		auto cmdList = m_renderer->CreateSingleUseCommandList(GpuQueueType::MAIN);
		cmdList->Start();

		// Los índices se guardan en una tabla de uint32:
		const USize64 singleIndexSize = sizeof(USize32);
		// La tabla es `attributesIndices`.
		const USize64 size	 = singleIndexSize * attributesIndices.GetSize();
		const USize64 offset = singleIndexSize * entry.firstVertex;

		auto stagingBuffer = m_renderer->GetAllocator()->CreateStagingBuffer(
			size,
			GpuQueueType::MAIN);

		stagingBuffer->MapMemory();
		stagingBuffer->Write(attributesIndices.GetData(), size);
		stagingBuffer->Unmap();

		cmdList->CopyBufferToBuffer(
			stagingBuffer.GetValue(),
			&m_bufferManager->GetBuffer(m_vertexAttribsIndexesBufferId.bufferUuid),
			size,
			0,			// <- offset en src
			offset);	// <- offset en destino

		cmdList->RegisterStagingBuffer(std::move(stagingBuffer));

		cmdList->Close();

		m_renderer->SubmitSingleUseCommandList(std::move(cmdList));
	}

	Engine::GetLogger()->DebugLog(std::format("Registrada geometría:"));
	Engine::GetLogger()->DebugLog(std::format("\tÍndices: de {} a {}", indicesEntry.firstIndex, indicesEntry.firstIndex + indicesEntry.indexCount));
	for (const auto& [name, attrib] : vertexEntriesPerAttrib) {
		Engine::GetLogger()->DebugLog(std::format("\tVértices({}): de {} a {}", name.Get(), indicesEntry.firstIndex, indicesEntry.firstIndex + indicesEntry.indexCount));
	}

	return GpuGeometryEntry(
		indicesEntry,
		entry,
		std::move(vertexEntriesPerAttrib));
}

std::span<const VertexAttributeEntry> RgGeometryManager::GetRegisteredAttribs() const {
	return m_attribs.GetFullSpan();
}

std::span<const GdrBufferUuid> RgGeometryManager::GetRegisteredAttribsBufferUuids() const {
	return m_attributesBufferUuids.GetFullSpan();
}

RgBufferRef RgGeometryManager::GetAttribsIndexes() const {
	return m_vertexAttribsIndexesBufferId;
}

RgBufferRef RgGeometryManager::GetIndexesBuffer() const {
	return m_indexesBufferId;
}

GlobalIndexBufferRange RgGeometryManager::RegisterGeometryIndexes(std::span<const TIndexSize> indexes) {
	const auto entry = m_indexesBufferManager.RegisterGeometry(indexes.size());

	auto cmdList = m_renderer->CreateSingleUseCommandList(GpuQueueType::MAIN);
	cmdList->Start();

	auto stagingBuffer = m_renderer->GetAllocator()->CreateStagingBuffer(
		sizeof(TIndexSize) * entry.indexCount,
		GpuQueueType::MAIN);

	stagingBuffer->MapMemory();
	stagingBuffer->Write(indexes.data(), indexes.size() * sizeof(TIndexSize));
	stagingBuffer->Unmap();

	cmdList->CopyBufferToBuffer(
		stagingBuffer.GetValue(),
		&m_bufferManager->GetBuffer(m_indexesBufferId.bufferUuid),
		sizeof(TIndexSize) * entry.indexCount,
		0,
		sizeof(TIndexSize) * entry.firstIndex);

	cmdList->RegisterStagingBuffer(std::move(stagingBuffer));

	cmdList->Close();

	m_renderer->SubmitSingleUseCommandList(std::move(cmdList));

	return entry;
}

void RgGeometryManager::PartiallyRegisterUnifiedGeometryBuffers() {
	m_vertexAttribsIndexesBufferId.bufferUuid = m_bufferManager->ReserveUuid();
	m_indexesBufferId.bufferUuid = m_bufferManager->ReserveUuid();

	m_bufferManager->AddBufferUse(m_vertexAttribsIndexesBufferId.bufferUuid, RgBufferUsage::TRANSFER_DST);
	m_bufferManager->AddBufferUse(m_indexesBufferId.bufferUuid, RgBufferUsage::TRANSFER_DST);
	m_bufferManager->AddBufferUse(m_indexesBufferId.bufferUuid, RgBufferUsage::INDEX_BUFFER);
}

void RgGeometryManager::RegisterUnifiedGeometryBuffers() {
	// Un buffer por cada tipo de atributo.
	for (const auto& [uuid, attrib] : m_attributesInfo) {
		RgBufferRegisterInfo attribBufferInfo{};
		attribBufferInfo.alignment = 0;
		attribBufferInfo.queue = GpuQueueType::MAIN;
		attribBufferInfo.sharedType = GpuSharedMemoryType::GPU_ONLY;
		attribBufferInfo.size = attrib.sizeOfAttrib * m_maxVertices;

		m_bufferManager->RegisterBuffer(attribBufferInfo, attrib.bufferRef);
		m_bufferManager->AddBufferUse(attrib.bufferRef, RgBufferUsage::TRANSFER_DST);

		const auto indexInsideTable = static_cast<UIndex32>(m_attribs.GetSize());

		m_attribs.Insert(VertexAttributeEntry{
			.name = attrib.name,
			.bufferRef = attrib.bufferRef,
			.sizeOfAttrib = static_cast<USize32>(attribBufferInfo.size),
			.indexInsideTable = indexInsideTable
			});
	}

	// Tablas de los índices de los atributos.
	RgBufferRegisterInfo attributesIndexesBufferInfo{};
	attributesIndexesBufferInfo.alignment = 0;
	attributesIndexesBufferInfo.queue = GpuQueueType::MAIN;
	attributesIndexesBufferInfo.sharedType = GpuSharedMemoryType::GPU_ONLY;
	attributesIndexesBufferInfo.size = sizeof(TIndexSize) * m_maxVertices * m_attributesInfo.size();
	m_bufferManager->RegisterBuffer(attributesIndexesBufferInfo, m_vertexAttribsIndexesBufferId.bufferUuid);

	// Buffer tradicional de índices.
	RgBufferRegisterInfo indexesBufferInfo{};
	indexesBufferInfo.alignment = 0;
	indexesBufferInfo.queue = GpuQueueType::MAIN;
	indexesBufferInfo.sharedType = GpuSharedMemoryType::GPU_ONLY;
	indexesBufferInfo.size = sizeof(TIndexSize) * m_maxVertices;
	m_bufferManager->RegisterBuffer(indexesBufferInfo, m_indexesBufferId.bufferUuid);
}

void RgGeometryManager::PrepareUnifiedGeometryBuffersReferences() {
	m_vertexAttribsIndexesBufferId.range = m_bufferManager->GetBuffer(m_vertexAttribsIndexesBufferId.bufferUuid).GetWholeBufferRange();
	m_indexesBufferId.range = m_bufferManager->GetBuffer(m_indexesBufferId.bufferUuid).GetWholeBufferRange();
}
