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
			UIndex32 familyIndex,
			UIndex32 inFamilyIndex,
			const GpuVk& gpu);

		VkQueue GetQueue() const;
		UIndex32 GetQueueIndex() const;
		UIndex32 GetFamilyIndex() const;

	private:

		VkQueue queue = nullptr;
		UIndex32 familyIndex = 0;
		UIndex32 inFamilyIndex = 0;

	};

}
