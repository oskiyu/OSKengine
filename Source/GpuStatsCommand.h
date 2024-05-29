#pragma once

#include "ApiCall.h"
#include "ConsoleCommand.h"

#include <string>
#include <span>

namespace OSK {

	class OSKAPI_CALL GpuStatsCommand : public IConsoleCommand {

	public:

		explicit GpuStatsCommand();

		void Execute(UI::Console* console, std::span<const std::string> params) const override;

	};

}
