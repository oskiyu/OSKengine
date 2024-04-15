#include "JobCountMap.h"

using namespace OSK;


void JobCountMap::Increment(std::string_view tag) {
	{
		std::shared_lock lock(m_mutex);

		if (m_map.contains(tag)) {
			m_map.find(tag)->second.fetch_add(1);
			return;
		}
	}

	// Si no estaba:
	{
		// Añadir.
		std::unique_lock lock(m_mutex);
		m_map[static_cast<std::string>(tag)] = 0;
	}

	// Incrementar.
	std::shared_lock lock(m_mutex);
	m_map.find(tag)->second.fetch_add(1);
}

void JobCountMap::Decrement(std::string_view tag) {
	// Incrementar.
	std::shared_lock lock(m_mutex);
	m_map.find(tag)->second.fetch_sub(1);
}

USize32 JobCountMap::GetCount(std::string_view tag) const {
	std::shared_lock lock(m_mutex);

	if (m_map.contains(tag)) {
		return m_map.find(tag)->second.load();
	}

	return 0;
}
