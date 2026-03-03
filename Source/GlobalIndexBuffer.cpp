#include "GlobalIndexBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

UIndex32 GlobalIndexBufferManager::GetNextIndexesStart(USize32 numIndexes) const {
	// Primero se comprueban los rangos libres.
	if (!m_unusedEntries.IsEmpty()) {
		for (UIndex64 i = 0; i < m_unusedEntries.GetSize(); i++) {
			const auto& entry = m_unusedEntries[i];

			if (entry.indexCount <= numIndexes) {
				return entry.firstIndex;
			}
		}
	}

	// Si no hay ningún rango libre disponible,
	// se devuelve el final del buffer.
	return static_cast<UIndex32>(m_tail);
}

GlobalIndexBufferRange GlobalIndexBufferManager::RegisterGeometry(USize32 numIndices) {

	// Primero se comprueban los rangos libres.
	if (!m_unusedEntries.IsEmpty()) {
		for (UIndex64 i = 0; i < m_unusedEntries.GetSize(); i++) {
			const auto& entry = m_unusedEntries[i];

			if (entry.indexCount <= numIndices) {
				GlobalIndexBufferRange output{};
				output.firstIndex = entry.firstIndex;
				output.indexCount = numIndices;

				if (numIndices < entry.indexCount) {
					m_unusedEntries.Insert(UnusedIndexBufferRange{
						.firstIndex = entry.firstIndex + numIndices,
						.indexCount = entry.indexCount - numIndices
						});
				}

				m_unusedEntries.RemoveAt(i);

				return output;
			}
		}
	}

	// Si no hay ningún rango libre disponible,
	// se coloca al final del buffer.
	GlobalIndexBufferRange output{};
	output.firstIndex = m_tail;
	output.indexCount = numIndices;

	m_tail += numIndices;

	m_entries.Insert(output);

	return output;
}
