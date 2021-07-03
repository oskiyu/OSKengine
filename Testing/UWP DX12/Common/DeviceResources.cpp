#include "pch.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

namespace DisplayMetrics
{
	// Las pantallas de alta resolución pueden requerir una gran cantidad de energía de la GPU y de la batería para realizar presentaciones.
	// Por ejemplo, la batería de los teléfonos de alta resolución puede durar muy poco si
	// se intenta presentar los juegos a 60 fotogramas por segundo con plena fidelidad.
	// La decisión de representar con plena fidelidad en todas las plataformas y factores de forma
	// debe ser deliberada.
	static const bool SupportHighResolutions = false;

	// Los umbrales predeterminados que definen una visualización de "alta resolución". Si estos umbrales
	// se sobrepasan y el valor de SupportHighResolutions es false, las dimensiones se escalarán
	// en un 50%.
	static const float DpiThreshold = 192.0f;		// 200% de la visualización de escritorio estándar.
	static const float WidthThreshold = 1920.0f;	// 1080p de ancho.
	static const float HeightThreshold = 1080.0f;	// 1080p de alto.
};

// Constantes usadas para calcular rotaciones de pantalla.
namespace ScreenRotation
{
	// Rotación Z de 0 grados
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	// Rotación Z de 90 grados
	static const XMFLOAT4X4 Rotation90(
		0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	// Rotación Z de 180 grados
	static const XMFLOAT4X4 Rotation180(
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	// Rotación Z de 270 grados
	static const XMFLOAT4X4 Rotation270(
		0.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
};

// Constructor para DeviceResources.
DX::DeviceResources::DeviceResources(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat) :
	m_currentFrame(0),
	m_screenViewport(),
	m_rtvDescriptorSize(0),
	m_fenceEvent(0),
	m_backBufferFormat(backBufferFormat),
	m_depthBufferFormat(depthBufferFormat),
	m_fenceValues{},
	m_d3dRenderTargetSize(),
	m_outputSize(),
	m_logicalSize(),
	m_nativeOrientation(DisplayOrientations::None),
	m_currentOrientation(DisplayOrientations::None),
	m_dpi(-1.0f),
	m_effectiveDpi(-1.0f),
	m_deviceRemoved(false)
{
	CreateDeviceIndependentResources();
	CreateDeviceResources();
}

// Configura los recursos que no dependen del dispositivo Direct3D.
void DX::DeviceResources::CreateDeviceIndependentResources()
{
}

// Configura el dispositivo Direct3D y almacena identificadores en este y en el contexto de dispositivo.
void DX::DeviceResources::CreateDeviceResources()
{
#if defined(_DEBUG)
	// Si el proyecto está en una compilación de depuración, habilite la depuración a través de capas SDK.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif

	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));

	ComPtr<IDXGIAdapter1> adapter;
	GetHardwareAdapter(&adapter);

	// Cree el objeto de dispositivo de API Direct3D 12
	HRESULT hr = D3D12CreateDevice(
		adapter.Get(),					// Adaptador del hardware.
		D3D_FEATURE_LEVEL_11_0,			// Nivel mínimo de funcionalidad que puede admitir esta aplicación.
		IID_PPV_ARGS(&m_d3dDevice)		// Devuelve el dispositivo Direct3D creado.
		);

#if defined(_DEBUG)
	if (FAILED(hr))
	{
		// Si no se puede inicializar, recurra al dispositivo WARP.
		// Para obtener más información sobre WARP, vea: 
		// https://go.microsoft.com/fwlink/?LinkId=286690

		ComPtr<IDXGIAdapter> warpAdapter;
		DX::ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
	}
#endif

	DX::ThrowIfFailed(hr);

	// Cree la cola de comandos.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DX::ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	NAME_D3D12_OBJECT(m_commandQueue);

	// Crear montones de descriptor para representar vistas de destino y vistas de galería de símbolos de profundidad.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = c_frameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DX::ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	NAME_D3D12_OBJECT(m_rtvHeap);

	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
	NAME_D3D12_OBJECT(m_dsvHeap);

	for (UINT n = 0; n < c_frameCount; n++)
	{
		DX::ThrowIfFailed(
			m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n]))
			);
	}

	// Cree objetos de sincronización.
	DX::ThrowIfFailed(m_d3dDevice->CreateFence(m_fenceValues[m_currentFrame], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValues[m_currentFrame]++;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
	{
		DX::ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
}

// Estos recursos se deben volver a crear cada vez que se cambie el tamaño de la ventana.
void DX::DeviceResources::CreateWindowSizeDependentResources()
{
	// Espere hasta que se complete todo el trabajo anterior de la GPU.
	WaitForGpu();

	// Borrar el contenido específico del tamaño de ventana anterior y actualizar los valores de la barrera objeto de seguimiento.
	for (UINT n = 0; n < c_frameCount; n++)
	{
		m_renderTargets[n] = nullptr;
		m_fenceValues[n] = m_fenceValues[m_currentFrame];
	}

	UpdateRenderTargetSize();

	// El ancho y el alto de la cadena de intercambio se debe basar en el
	// ancho y el alto de la ventana en orientación nativa. Si la ventana no tiene orientación
	// nativa, se deberán invertir las dimensiones.
	DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation();

	bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	m_d3dRenderTargetSize.Width = swapDimensions ? m_outputSize.Height : m_outputSize.Width;
	m_d3dRenderTargetSize.Height = swapDimensions ? m_outputSize.Width : m_outputSize.Height;

	UINT backBufferWidth = lround(m_d3dRenderTargetSize.Width);
	UINT backBufferHeight = lround(m_d3dRenderTargetSize.Height);

	if (m_swapChain != nullptr)
	{
		// Si la cadena de intercambio ya existe, cambie el tamaño.
		HRESULT hr = m_swapChain->ResizeBuffers(c_frameCount, backBufferWidth, backBufferHeight, m_backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// Si el dispositivo se quitó por cualquier motivo, se deberá crear un dispositivo y una cadena de intercambio nuevos.
			m_deviceRemoved = true;

			// No prosiga con la ejecución de este método. DeviceResources se destruirá y se volverá a crear.
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		// De lo contrario, cree una nueva con el mismo adaptador que el del dispositivo Direct3D existente.
		DXGI_SCALING scaling = DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

		swapChainDesc.Width = backBufferWidth;						// Haga coincidir el tamaño de la ventana.
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = m_backBufferFormat;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;							// No usar varios ejemplos.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = c_frameCount;					// Use el almacenamiento en búfer triple para minimizar la latencia.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// Todas las aplicaciones universales de Windows deben utilizar _FLIP_ SwapEffects
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = scaling;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		ComPtr<IDXGISwapChain1> swapChain;
		DX::ThrowIfFailed(
			m_dxgiFactory->CreateSwapChainForCoreWindow(
				m_commandQueue.Get(),								// Las cadenas de intercambio necesitan una referencia a la cola de comandos en DirectX 12.
				reinterpret_cast<IUnknown*>(m_window.Get()),
				&swapChainDesc,
				nullptr,
				&swapChain
				)
			);

		DX::ThrowIfFailed(swapChain.As(&m_swapChain));
	}

	// Establezca la orientación adecuada para la cadena de intercambio y genere
	// 3D para la representación en la cadena de intercambio girada.
	// La matriz 3D se ha especificado explícitamente para evitar errores de redondeo.

	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		m_orientationTransform3D = ScreenRotation::Rotation0;
		break;

	case DXGI_MODE_ROTATION_ROTATE90:
		m_orientationTransform3D = ScreenRotation::Rotation270;
		break;

	case DXGI_MODE_ROTATION_ROTATE180:
		m_orientationTransform3D = ScreenRotation::Rotation180;
		break;

	case DXGI_MODE_ROTATION_ROTATE270:
		m_orientationTransform3D = ScreenRotation::Rotation90;
		break;

	default:
		throw ref new FailureException();
	}

	DX::ThrowIfFailed(
		m_swapChain->SetRotation(displayRotation)
		);

	// Crear vistas de destino de representación del búfer de reserva de la cadena de intercambio.
	{
		m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < c_frameCount; n++)
		{
			DX::ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_d3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvDescriptor);
			rtvDescriptor.Offset(m_rtvDescriptorSize);

			WCHAR name[25];
			if (swprintf_s(name, L"m_renderTargets[%u]", n) > 0)
			{
				DX::SetName(m_renderTargets[n].Get(), name);
			}
		}
	}

	// Crear una vista y una galería de símbolos de profundidad.
	{
		D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1);
		depthResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		CD3DX12_CLEAR_VALUE depthOptimizedClearValue(m_depthBufferFormat, 1.0f, 0);

		ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
			&depthHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&m_depthStencil)
			));

		NAME_D3D12_OBJECT(m_depthStencil);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = m_depthBufferFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	// Establezca la ventanilla de representación en 3D para que afecte a toda la ventana.
	m_screenViewport = { 0.0f, 0.0f, m_d3dRenderTargetSize.Width, m_d3dRenderTargetSize.Height, 0.0f, 1.0f };
}

// Determina las dimensiones del destino de representación y si se va a reducir.
void DX::DeviceResources::UpdateRenderTargetSize()
{
	m_effectiveDpi = m_dpi;

	// Para prolongar la duración de la batería en los dispositivos de alta resolución, use un destino de representación más pequeño
	// y permita que la GPU escale la salida en la presentación.
	if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold)
	{
		float width = DX::ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
		float height = DX::ConvertDipsToPixels(m_logicalSize.Height, m_dpi);

		// Cuando el dispositivo tiene orientación vertical, el alto es superior al ancho. Compare la
		// dimensión más grande con el umbral de ancho y la dimensión más pequeña
		// con el umbral de alto.
		if (max(width, height) > DisplayMetrics::WidthThreshold && min(width, height) > DisplayMetrics::HeightThreshold)
		{
			// Para escalar la aplicación, se cambia el valor PPP efectivo. El tamaño lógico no cambia.
			m_effectiveDpi /= 2.0f;
		}
	}

	// Calcular el tamaño del destino de presentación necesario en píxeles.
	m_outputSize.Width = DX::ConvertDipsToPixels(m_logicalSize.Width, m_effectiveDpi);
	m_outputSize.Height = DX::ConvertDipsToPixels(m_logicalSize.Height, m_effectiveDpi);

	// Impedir la creación de contenido de DirectX de tamaño cero.
	m_outputSize.Width = max(m_outputSize.Width, 1);
	m_outputSize.Height = max(m_outputSize.Height, 1);
}

// Este método se llama cuando se crea (o se vuelve a crear) el objeto CoreWindow.
void DX::DeviceResources::SetWindow(CoreWindow^ window)
{
	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	m_window = window;
	m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
	m_nativeOrientation = currentDisplayInformation->NativeOrientation;
	m_currentOrientation = currentDisplayInformation->CurrentOrientation;
	m_dpi = currentDisplayInformation->LogicalDpi;

	CreateWindowSizeDependentResources();
}

// Este método se llama en el controlador de eventos del evento SizeChanged.
void DX::DeviceResources::SetLogicalSize(Windows::Foundation::Size logicalSize)
{
	if (m_logicalSize != logicalSize)
	{
		m_logicalSize = logicalSize;
		CreateWindowSizeDependentResources();
	}
}

// Este método se llama en el controlador de eventos del evento DpiChanged.
void DX::DeviceResources::SetDpi(float dpi)
{
	if (dpi != m_dpi)
	{
		m_dpi = dpi;

		// Al cambiar el valor PPP de la pantalla, también cambia el tamaño lógico de la ventana (que se mide en PID) y se debe actualizar.
		m_logicalSize = Windows::Foundation::Size(m_window->Bounds.Width, m_window->Bounds.Height);

		CreateWindowSizeDependentResources();
	}
}

// Este método se llama en el controlador de eventos del evento OrientationChanged.
void DX::DeviceResources::SetCurrentOrientation(DisplayOrientations currentOrientation)
{
	if (m_currentOrientation != currentOrientation)
	{
		m_currentOrientation = currentOrientation;
		CreateWindowSizeDependentResources();
	}
}

// Este método se llama en el controlador de eventos del evento DisplayContentsInvalidated.
void DX::DeviceResources::ValidateDevice()
{
	// El dispositivo D3D deja de ser válido si el adaptador predeterminado cambió desde que el dispositivo
	// se creó o el dispositivo se quitadó.

	// En primer lugar, debe obtenerse el LUID del adaptador predeterminado con el que se creó el dispositivo.

	DXGI_ADAPTER_DESC previousDesc;
	{
		ComPtr<IDXGIAdapter1> previousDefaultAdapter;
		DX::ThrowIfFailed(m_dxgiFactory->EnumAdapters1(0, &previousDefaultAdapter));

		DX::ThrowIfFailed(previousDefaultAdapter->GetDesc(&previousDesc));
	}

	// A continuación, obtenga la información para el adaptador predeterminado actual.

	DXGI_ADAPTER_DESC currentDesc;
	{
		ComPtr<IDXGIFactory4> currentDxgiFactory;
		DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentDxgiFactory)));

		ComPtr<IDXGIAdapter1> currentDefaultAdapter;
		DX::ThrowIfFailed(currentDxgiFactory->EnumAdapters1(0, &currentDefaultAdapter));

		DX::ThrowIfFailed(currentDefaultAdapter->GetDesc(&currentDesc));
	}

	// Si los identificadores únicos locales (LUID) del adaptador no coinciden o el dispositivo notifica que se ha quitado,
	// se debe crear un nuevo dispositivo D3D.

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(m_d3dDevice->GetDeviceRemovedReason()))
	{
		m_deviceRemoved = true;
	}
}

// Presente el contenido de la cadena de intercambio en la pantalla.
void DX::DeviceResources::Present()
{
	// El primer argumento indica a DXGI que se bloquee hasta VSync, lo que pone a la aplicación
	// en suspensión hasta el siguiente VSync. Esto asegura que no se desperdician ciclos presentando
	// fotogramas que no se mostrarán nunca en la pantalla.
	HRESULT hr = m_swapChain->Present(1, 0);

	// Si el dispositivo se quitó por una desconexión o una actualización del controlador, 
	// debe volver a crear todos los recursos de dispositivo.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		m_deviceRemoved = true;
	}
	else
	{
		DX::ThrowIfFailed(hr);

		MoveToNextFrame();
	}
}

// Espere hasta que se complete el trabajo pendiente de la GPU.
void DX::DeviceResources::WaitForGpu()
{
	// Programe un comando de señal en la cola.
	DX::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_currentFrame]));

	// Espere hasta que se haya cruzado la barrera.
	DX::ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrame], m_fenceEvent));
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// Aumente el valor de barrera del marco actual.
	m_fenceValues[m_currentFrame]++;
}

// Prepare la representación del siguiente marco.
void DX::DeviceResources::MoveToNextFrame()
{
	// Programe un comando de señal en la cola.
	const UINT64 currentFenceValue = m_fenceValues[m_currentFrame];
	DX::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

	// Avance el índice del marco.
	m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();

	// Compruebe si ya se puede iniciar el siguiente marco.
	if (m_fence->GetCompletedValue() < m_fenceValues[m_currentFrame])
	{
		DX::ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrame], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// Establezca el valor de barrera del siguiente marco.
	m_fenceValues[m_currentFrame] = currentFenceValue + 1;
}

// Este método determina la rotación entre la orientación nativa del dispositivo de pantalla y
// la orientación de pantalla actual.
DXGI_MODE_ROTATION DX::DeviceResources::ComputeDisplayRotation()
{
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

	// Nota: NativeOrientation solo puede tener los valores Landscape o Portrait aunque
	// la enumeración DisplayOrientations tiene otros valores.
	switch (m_nativeOrientation)
	{
	case DisplayOrientations::Landscape:
		switch (m_currentOrientation)
		{
		case DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;

		case DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;
		}
		break;

	case DisplayOrientations::Portrait:
		switch (m_currentOrientation)
		{
		case DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;

		case DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;
		}
		break;
	}
	return rotation;
}

// Este método adquiere el primer adaptador de hardware disponible que admita Direct3D 12.
// Si no se encuentra ningún adaptador de este tipo, *ppAdapter se establecerá en nullptr.
void DX::DeviceResources::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// No seleccione el adaptador Controlador de representación básica.
			continue;
		}

		// Compruebe si el adaptador admite Direct3D 12, pero no cree
		// aún el dispositivo en sí.
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	*ppAdapter = adapter.Detach();
}
