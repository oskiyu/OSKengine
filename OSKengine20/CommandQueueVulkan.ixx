#include <vulkan/vulkan.h>

export module OSKengine.Graphics.CommandQueueVulkan;

export import OSKengine.Graphics.ICommandQueue;

export namespace OSK {

	class CommandQueueVulkan : public ICommandQueue {

	public:

		void SetQueue(VkQueue q) {
			this->queue = q;
		}

	private:

		VkQueue queue;

	};

}