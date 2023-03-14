#pragma once

#include "ICommandQueue.h"

struct VkQueue_T;
typedef VkQueue_T* VkQueue;

namespace OSK::GRAPHICS {

	class GpuVk;

	class OSKAPI_CALL CommandQueueVk final : public ICommandQueue {

	public:

		CommandQueueVk(
			CommandQueueSupport support, 
			TIndex familyIndex, 
			TIndex inFamilyIndex, 
			const GpuVk& gpu);

		VkQueue GetQueue() const;
		TIndex GetQueueIndex() const;
		TIndex GetFamilyIndex() const;

	private:

		VkQueue queue = 0;
		TIndex familyIndex = 0;
		TIndex inFamilyIndex = 0;

	};

}
