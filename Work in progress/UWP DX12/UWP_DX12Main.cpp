#include "pch.h"
#include "UWP_DX12Main.h"
#include "Common\DirectXHelper.h"

using namespace UWP_DX12;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// La plantilla de la aplicaci�n DirectX 12 est� documentada en https://go.microsoft.com/fwlink/?LinkID=613670&clcid=0xc0a

// Carga e inicializa los activos de la aplicaci�n cuando se carga la aplicaci�n.
UWP_DX12Main::UWP_DX12Main()
{
	// TODO: Cambie la configuraci�n del temporizador si desea usar un modo distinto al modo de timestep variable predeterminado.
	// p. ej. para una l�gica de actualizaci�n de timestep fijo de 60 FPS, llame a:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

// Crea e inicializa los representadores.
void UWP_DX12Main::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	// TODO: Reempl�celo por la inicializaci�n del contenido de su aplicaci�n.
	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(deviceResources));

	OnWindowSizeChanged();
}

// Actualiza el estado de la aplicaci�n una vez por marco.
void UWP_DX12Main::Update()
{
	// Actualizar los objetos de la escena.
	m_timer.Tick([&]()
	{
		// TODO: Reempl�celo por las funciones de actualizaci�n de contenido de su aplicaci�n.
		m_sceneRenderer->Update(m_timer);
	});
}

// Presenta el marco actual de acuerdo con el estado actual de la aplicaci�n.
// Devuelve true si se ha presentado el marco y est� listo para ser mostrado.
bool UWP_DX12Main::Render()
{
	// No intente presentar nada antes de la primera actualizaci�n.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	// Presentar los objetos de la escena.
	// TODO: Reempl�celo por las funciones de representaci�n de contenido de su aplicaci�n.
	return m_sceneRenderer->Render();
}

// Actualiza el estado de la aplicaci�n cuando cambia el tama�o de la ventana (por ejemplo, al cambiar la orientaci�n del dispositivo)
void UWP_DX12Main::OnWindowSizeChanged()
{
	// TODO: Reempl�celo por la inicializaci�n dependiente del tama�o del contenido de su aplicaci�n.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

// Notifica la aplicaci�n que se va a suspender.
void UWP_DX12Main::OnSuspending()
{
	// TODO: Reemplace este valor por la l�gica de suspensi�n de su aplicaci�n.

	// La Administraci�n del ciclo de vida de los procesos puede finalizar las aplicaciones suspendidas en cualquier momento, por lo que
	// se recomienda guardar todos los estados que permitan reiniciar la aplicaci�n donde se detuvo.

	m_sceneRenderer->SaveState();

	// Si su aplicaci�n utiliza asignaciones de memoria de v�deo que se pueden volver a crear f�cilmente,
	// considere la posibilidad de liberar dicha memoria para que est� disponible para otras aplicaciones.
}

// Notifica la aplicaci�n que ya no est� en suspensi�n.
void UWP_DX12Main::OnResuming()
{
	// TODO: Reemplace este valor por la l�gica de reanudaci�n de su aplicaci�n.
}

// Notifica a los representadores que deben liberarse recursos del dispositivo.
void UWP_DX12Main::OnDeviceRemoved()
{
	// TODO: Guarde cualquier estado del representador y aplicaci�n necesaria y libere el representador
	// y los recursos que ya no sean v�lidos.
	m_sceneRenderer->SaveState();
	m_sceneRenderer = nullptr;
}
