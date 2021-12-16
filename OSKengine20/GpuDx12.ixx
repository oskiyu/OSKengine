#include <wrl.h>
using namespace Microsoft::WRL; 

#include <d3d12.h>
#include <dxgi1_6.h>

export module OSKengine.Graphics.GpuDx12;

export import OSKengine.Graphics.IGpu;

export namespace OSK {

	class GpuDx12 : public IGpu {

	public:

		void SetAdapter(const ComPtr<IDXGIAdapter1>& adapter) {
			this->adapter = adapter;
		}

		void SetDevice(const ComPtr<ID3D12Device>& device) {
			this->device = device;
		}

		IDXGIAdapter1* GetAdapter() const {
			return adapter.Get();
		}

		ID3D12Device* GetDevice() const {
			return device.Get();
		}

	private:

		ComPtr<IDXGIAdapter1> adapter;
		ComPtr<ID3D12Device> device;

	};

}
