#include "DetailedCollisionDetectorDispatcher.h"

#include "Assert.h"
#include "UnreachableException.h"

void OSK::COLLISION::DetailedCollisionDetectorDispatcher::AddDetector(UniquePtr<IDetailedCollisionDetector>&& detector) {
	m_detectors.Insert(std::move(detector));
}

OSK::COLLISION::DetailedCollisionInfo OSK::COLLISION::DetailedCollisionDetectorDispatcher::GetCollision(
	const NarrowColliderHolder& first,
	const NarrowColliderHolder& second) const
{
	// Dar preferencia a los últimos añadidos.
	for (UIndex64 i = 0; i < m_detectors.GetSize(); i++) {
		const auto collision = m_detectors[m_detectors.GetSize() - 1 - i]->GetCollision(first, second);

		if (collision.has_value()) {
			return collision.value();
		}
	}

	// No debería ocurrir.
	OSK_ASSERT(false, UnreachableException("No se ha encontrado ningún detector compatible."));
}
