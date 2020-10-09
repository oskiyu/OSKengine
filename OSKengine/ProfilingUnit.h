#pragma once

#include "OSKtypes.h"

#include <string>

namespace OSK {

	struct ProfilingUnit {

	public:

		ProfilingUnit();
		ProfilingUnit(const std::string& name);

		void Start();
		void End();

		deltaTime_t GetValue() const;

		std::string Name = "UNKNOWN";

	private:

		deltaTime_t startTime = 0;
		deltaTime_t endTime = 0;
		deltaTime_t lastDuration = 0;

	};

}
