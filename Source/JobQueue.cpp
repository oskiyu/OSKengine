#include "JobQueue.h"

using namespace OSK;


JobQueue::JobQueue(USize32 maxElements) {
	m_jobs.Resize(maxElements, nullptr);
}

void JobQueue::Enqueue(UniquePtr<IJob> job) {
	std::lock_guard scopedLock(m_lock);

	const UIndex64 nextIdx = (m_head + 1) % m_jobs.GetSize();

	if (nextIdx == m_tail) {
		Resize();
	}

	m_jobs[m_head] = std::move(job);
	m_head = nextIdx;
}

std::optional<UniquePtr<IJob>> JobQueue::TryDequeue() {
	std::lock_guard scopedLock(m_lock);

	if (m_head == m_tail) {
		return {};
	}

	IJob* output = m_jobs[m_tail].Release();
	m_tail = (m_tail + 1) % m_jobs.GetSize();

	return output;
}

bool JobQueue::IsEmpty() const {
	std::lock_guard scopedLock(m_lock);

	return m_head == m_tail;
}

void JobQueue::Resize() {
	DynamicArray<UniquePtr<IJob>> jobs{};

	if (m_tail != m_head) {
		UIndex64 ptr = m_tail;

		while (ptr != m_head) {
			jobs.Insert(m_jobs[ptr].Release());
			ptr = (ptr + 1) % m_jobs.GetSize();
		}
	}

	m_jobs.Resize(m_jobs.GetSize() * 2);

	m_head = 0;
	m_tail = 0;

	for (auto& job : jobs) {
		m_jobs[m_head] = job.Release();
		m_head = (m_head + 1) % m_jobs.GetSize();
	}
}
