#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>

export module OSKengine.Graphics.CommandQueueDx12;

export import OSKengine.Graphics.ICommandQueue;

export namespace OSK {

	class CommandQueueDx12 : public ICommandQueue {

	public:

		void SetCommandQueue(const ComPtr<ID3D12CommandQueue>& commandQ) {
			this->commandQ = new ComPtr<ID3D12CommandQueue>(commandQ);
		}

		ID3D12CommandQueue* GetCommandQueue() {
			return commandQ.Get();
		}

	private:

		ComPtr<ID3D12CommandQueue> commandQ;

	};

}
