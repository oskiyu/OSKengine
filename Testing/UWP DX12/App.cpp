#include "pch.h"
#include "App.h"

#include <ppltasks.h>

using namespace UWP_DX12;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

using Microsoft::WRL::ComPtr;

// La plantilla de la aplicación DirectX 12 está documentada en https://go.microsoft.com/fwlink/?LinkID=613670&clcid=0xc0a

// La función principal solo se utiliza para inicializar nuestra clase IFrameworkView.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// El primer método al que se llama cuando se crea IFrameworkView.
void App::Initialize(CoreApplicationView^ applicationView)
{
	// Registrar controladores de eventos del ciclo de vida de la aplicación. Este ejemplo incluye Activated, así que
	// podemos activar CoreWindow e iniciar la representación en la ventana.
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);
}

// Se llama cuando se crea o se vuelve a crear el objeto CoreWindow.
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);
}

// Inicializa los recursos de la escena o carga un estado de aplicación previamente guardado.
void App::Load(Platform::String^ entryPoint)
{
	if (m_main == nullptr)
	{
		m_main = std::unique_ptr<UWP_DX12Main>(new UWP_DX12Main());
	}
}

// Se llama a este método una vez que se activa la ventana.
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			auto commandQueue = GetDeviceResources()->GetCommandQueue();
			PIXBeginEvent(commandQueue, 0, L"Update");
			{
				m_main->Update();
			}
			PIXEndEvent(commandQueue);

			PIXBeginEvent(commandQueue, 0, L"Render");
			{
				if (m_main->Render())
				{
					GetDeviceResources()->Present();
				}
			}
			PIXEndEvent(commandQueue);
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// Necesario para IFrameworkView.
// Cuando finalizan los eventos, no se llama a Uninitialize. Se llamará si su clase IFrameworkView
// está desactivada mientras la aplicación está en primer plano.
void App::Uninitialize()
{
}

// Controladores de eventos del ciclo de vida de la aplicación.

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() no se iniciará hasta que se active CoreWindow.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Guarde el estado de la aplicación de forma asincrónica tras solicitar un aplazamiento. Cuando se efectúa un aplazamiento,
	// se indica que la aplicación está ocupada realizando operaciones de suspensión. Tenga en cuenta que
	// es posible que los aplazamientos no se realicen de forma indefinida. Transcurridos unos cinco segundos,
	// la aplicación se verá forzada a salir.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
		m_main->OnSuspending();
		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restaure cualquier dato o estado que se haya descargado durante la suspensión. De forma predeterminada, los datos y
	// estados persisten al reanudar tras una suspensión. Tenga en cuenta que este evento
	// no se produce si se ha finalizado previamente la aplicación.

	m_main->OnResuming();
}

// Controladores de eventos de Windows.

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	GetDeviceResources()->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
	m_main->OnWindowSizeChanged();
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

// Controladores de eventos DisplayInformation.

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	// Nota: Puede que el valor que se recupera aquí para LogicalDpi no coincida con el valor PPP real de la aplicación
	// si se va a escalar para dispositivos de alta resolución. Una vez establecido el valor PPP en DeviceResources,
	// use siempre el método GetDpi para recuperarlo.
	// Consulte DeviceResources.cpp para obtener más detalles.
	GetDeviceResources()->SetDpi(sender->LogicalDpi);
	m_main->OnWindowSizeChanged();
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	GetDeviceResources()->SetCurrentOrientation(sender->CurrentOrientation);
	m_main->OnWindowSizeChanged();
}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	GetDeviceResources()->ValidateDevice();
}

std::shared_ptr<DX::DeviceResources> App::GetDeviceResources()
{
	if (m_deviceResources != nullptr && m_deviceResources->IsDeviceRemoved())
	{
		// Todas las referencias al dispositivo D3D existente deben liberarse antes de que se pueda crear
		// un dispositivo nuevo.

		m_deviceResources = nullptr;
		m_main->OnDeviceRemoved();

#if defined(_DEBUG)
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
#endif
	}

	if (m_deviceResources == nullptr)
	{
		m_deviceResources = std::make_shared<DX::DeviceResources>();
		m_deviceResources->SetWindow(CoreWindow::GetForCurrentThread());
		m_main->CreateRenderers(m_deviceResources);
	}
	return m_deviceResources;
}
