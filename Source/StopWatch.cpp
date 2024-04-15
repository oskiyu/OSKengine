#include "StopWatch.h"

#include "OSKengine.h"

using namespace OSK;


void StopWatch::Start() {
	m_startTime = Engine::GetCurrentTime();
}

void StopWatch::Stop() {
	m_endTime = Engine::GetCurrentTime();
}

TDeltaTime StopWatch::GetElapsedTime() const {
	return m_state == State::RUNNING
		? Engine::GetCurrentTime() - m_startTime
		: m_endTime - m_startTime;
}

StopWatch::State StopWatch::GetCurrentState() const {
	return m_state;
}
