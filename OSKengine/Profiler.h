#pragma once

#include "OSKtypes.h"

#include "ProfilingUnit.h"
#include <vector>

namespace OSK {

	class Profiler {

	public:

		void AddProfilingUnit(ProfilingUnit* unit);
		void RemoveProfilingUnit(ProfilingUnit* unit);

		virtual void ShowData();

	private:

		std::vector<ProfilingUnit*> Units{};

	};

}
