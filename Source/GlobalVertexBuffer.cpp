#include "GlobalVertexBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GlobalVertexBufferRange GlobalVertexBufferManager::RegisterGeometry(USize32 vertexCount) {
	if (!m_unusedEntries.IsEmpty()) {
		for (UIndex64 i = 0; i < m_unusedEntries.GetSize(); i++) {
			const auto& entry = m_unusedEntries[i];

			if (entry.vertexCount <= vertexCount) {
				GlobalVertexBufferRange output{};
				output.firstVertex = entry.firstVertex;
				output.vertexCount = vertexCount;

				if (vertexCount < entry.vertexCount) {
					m_unusedEntries.Insert(UnusedVertexBufferRange{
						.firstVertex = entry.firstVertex + vertexCount,
						.vertexCount = entry.vertexCount - vertexCount
						});
				}

				m_unusedEntries.RemoveAt(i);

				return output;
			}
		}
	}

	GlobalVertexBufferRange output{};
	output.firstVertex = m_tail;
	output.vertexCount = vertexCount;

	m_tail += vertexCount;

	m_entries.Insert(output);

	return output;
}
