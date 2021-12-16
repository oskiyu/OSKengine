#pragma once

#include "IGpuMemorySubblock.h"

namespace OSK {

	class OSKAPI_CALL GpuMemorySubblockVulkan : public IGpuMemorySubblock {

	public:

		GpuMemorySubblockVulkan(IGpuMemoryBlock* owner, TSize size, TSize offset);

		void MapMemory() override;
		void MapMemory(TSize size, TSize offset) override;
		void Write(const void* data, TSize size) override;
		void WriteOffset(const void* data, TSize size, TSize offset) override;
		void Unmap() override;

	};

}
