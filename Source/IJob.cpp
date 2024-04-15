#include "IJob.h"

using namespace OSK;


void IJob::AddTag(const std::string& tag) {
	m_tags.insert(tag);
}

const std::set<std::string, std::less<>>& IJob::GetTags() const {
	return m_tags;
}
