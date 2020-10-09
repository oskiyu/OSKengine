#include "Profiler.h"

using namespace OSK;

void Profiler::AddProfilingUnit(ProfilingUnit* unit) {
	for (const auto& i : Units)
		if (i == unit)
			return;

	Units.push_back(unit);
}

void Profiler::RemoveProfilingUnit(ProfilingUnit* unit) {

}

void Profiler::ShowData() {
	Logger::Log(LogMessageLevels::INFO, "PROFILER: ");

	for (const auto& i : Units) {
		Logger::Log(LogMessageLevels::INFO, "\t" + i->Name + ": " + std::to_string(i->GetValue() * 1000) + "ms");
	}
}