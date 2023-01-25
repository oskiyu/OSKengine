#pragma once

#include "ICommandQueue.h"

struct VkQueue_T;
typedef VkQueue_T* VkQueue;

namespace OSK::GRAPHICS {

	class GpuVulkan;

	class OSKAPI_CALL CommandQueueVulkan : public ICommandQueue {

	public:

		CommandQueueVulkan(CommandQueueSupport support, TIndex familyIndex, TIndex inFamilyIndex, const GpuVulkan& gpu);
		VkQueue GetQueue() const;
		TIndex GetQueueIndex() const;
		TIndex GetFamilyIndex() const;

	private:

		VkQueue queue = 0;
		TIndex familyIndex = 0;
		TIndex inFamilyIndex = 0;

	};

}
