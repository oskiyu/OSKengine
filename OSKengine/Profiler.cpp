#include "Profiler.h"

using namespace OSK;

void Profiler::AddProfilingUnit(ProfilingUnit* unit) {
	for (const auto& i : units)
		if (i == unit)
			return;

	units.push_back(unit);
}

void Profiler::RemoveProfilingUnit(ProfilingUnit* unit) {

}

void Profiler::ShowData() {
	Logger::Log(LogMessageLevels::INFO, "PROFILER: ");

	for (const auto& i : units) {
		Logger::Log(LogMessageLevels::INFO, "\t" + i->name + ": " + std::to_string(i->GetValue() * 1000) + "ms");
	}
}