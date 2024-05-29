#include "GpuStatsCommand.h"

#include "UiConsole.h"

#include "ApiCall.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryUsageInfo.h"

#include <span>
#include <string>
#include <format>

using namespace OSK;


GpuStatsCommand::GpuStatsCommand() {
	SetCommandWords({ "gpu", "mem", "info" });
}

void GpuStatsCommand::Execute(UI::Console* console, std::span<const std::string> params) const {
	const auto stats = Engine::GetRenderer()->GetAllocator()->GetMemoryUsageInfo();

	constexpr static USize32 bytesPerMb = 1000000;

	console->AddMessage(
		std::format("VRAM.: {}/{} (MB)", 
			stats.gpuOnlyMemoryInfo.usedSpace / bytesPerMb,
			stats.gpuOnlyMemoryInfo.maxCapacity / bytesPerMb));
}
