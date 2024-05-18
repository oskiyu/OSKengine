#include "SavedGameObjectTranslator.h"

using namespace OSK;
using namespace OSK::ECS;

void SavedGameObjectTranslator::AddObject(GameObjectIndex savedIndex, GameObjectIndex currentIndex) {
	m_savedToCurrent[savedIndex] = currentIndex;
}

GameObjectIndex SavedGameObjectTranslator::GetCurrentIndex(GameObjectIndex savedIndex) const {
	if (savedIndex.IsEmpty()) {
		return GameObjectIndex::CreateEmpty();
	}

	return m_savedToCurrent.at(savedIndex);
}

bool SavedGameObjectTranslator::HasSavedIndex(GameObjectIndex savedIndex) const {
	return m_savedToCurrent.contains(savedIndex);
}
